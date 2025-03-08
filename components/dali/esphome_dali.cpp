#include <esphome.h>
#include "esphome_dali.h"
#include "esphome_dali_light.h"

//static const char *const TAG = "dali";
static const bool DEBUG_LOG_RXTX = false;

using namespace esphome;
using namespace dali;

void DaliBusComponent::setup() {
    m_txPin->pin_mode(gpio::Flags::FLAG_OUTPUT);

    if (m_discovery) {
        DALI_LOGI("Begin device discovery...");

        // if (dali.bus_manager.isControlGearPresent()) {
        //     DALI_LOGD("Detected control gear on bus");
        // } else {
        //     DALI_LOGW("No control gear detected on bus!");
        // }

        if (this->m_initialize_addresses) {
            DALI_LOGI("Randomizing addresses for unassigned DALI devices");
            // Only randomize devices without an assigned short address
            dali.bus_manager.initialize(ASSIGN_UNINITIALIZED); 
            //dali.bus_manager.initialize(ASSIGN_ALL); 
            dali.bus_manager.randomize();
            dali.bus_manager.terminate();
            // Seem to need a delay to allow time for devices to randomize...
        }

        dali.bus_manager.startAddressScan(); // All devices

        for (int i = 0; i < 64; i++) {
            m_discovered_addresses[i] = 0;
        }

        // TODO: Device discovery doesn't seem to be working correctly with two devices attached.
        // Unsure if it is my physical circuit, or a bug in the discovery algorithm.
        //
        // It seems that both devices get assigned different random addresses,
        // and the search correctly finds the lowest address,
        // but when calling withdraw() on the lower address both stop responding.
        // 
        uint8_t count = 0;
        short_addr_t short_addr = 0xFF;
        uint32_t long_addr = 0;
        while (dali.bus_manager.findNextAddress(short_addr, long_addr)) {

            if (short_addr == 0xFF) {
                if (this->m_initialize_addresses) {
                    dali.bus_manager.programShortAddress(count);
                    short_addr = count;
                }
                else {
                    DALI_LOGW("  no short address assigned!");
                }
            }

            DALI_LOGI("  Device %.6x @ %.2x", long_addr, short_addr);

            if (short_addr <= 64) {
                if (m_discovered_addresses[short_addr] != 0) {
                    DALI_LOGW("WARNING: Duplicate short address detected!");
                }

                m_discovered_addresses[short_addr] = long_addr;
                create_light_component(short_addr, long_addr);
            }

            delay(1); // yield to ESP stack
            // TODO: Also reset task WDT
            count++;
        }

        DALI_LOGD("No more devices found!");
        dali.bus_manager.endAddressScan();
    }
}

void DaliBusComponent::create_light_component(short_addr_t short_addr, uint32_t long_addr) {
    DaliLight* dali_light = new DaliLight { this };
    dali_light->set_address(short_addr);

    const int MAX_STR_LEN = 20;
    char* name = new char[MAX_STR_LEN];
    char* id = new char[MAX_STR_LEN];
    snprintf(name, MAX_STR_LEN, "DALI Light %d", short_addr);
    snprintf(id, MAX_STR_LEN, "dali_light_%.6x", long_addr);
    // NOTE: Not freeing these strings, they will be owned by LightState.

    auto* light_state = new light::LightState { dali_light };
    light_state->set_component_source("light");
    App.register_light(light_state);
    App.register_component(light_state);
    light_state->set_name(name);
    light_state->set_object_id(id);
    light_state->set_disabled_by_default(false);
    light_state->set_restore_mode(light::LIGHT_RESTORE_DEFAULT_ON);
    light_state->add_effects({});

    DALI_LOGI("Created light component '%s' (%s)", name, id);
}

void DaliBusComponent::loop() {

}

void DaliBusComponent::dump_config() {

}

#define QUARTER_BIT_PERIOD 208
#define HALF_BIT_PERIOD 416
#define BIT_PERIOD 833

void DaliBusComponent::writeBit(bool bit) {
    // NOTE: output is inverted - HIGH will pull the bus to 0V (logic low)
    bit = !bit;
    m_txPin->digital_write(bit ? LOW : HIGH);
    delayMicroseconds(HALF_BIT_PERIOD-6);
    m_txPin->digital_write(bit ? HIGH : LOW);
    delayMicroseconds(HALF_BIT_PERIOD-6);
}

void DaliBusComponent::writeByte(uint8_t b) {
    for (int i = 0; i < 8; i++) {
        writeBit(b & 0x80);
        b <<= 1;
    }
}

uint8_t DaliBusComponent::readByte() {
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++) {
        byte <<= 1;
        byte |= m_rxPin->digital_read();
        delayMicroseconds(BIT_PERIOD); // 1/1200 seconds
    }
    return byte;
}

void DaliBusComponent::sendForwardFrame(uint8_t address, uint8_t data) {
    if (DEBUG_LOG_RXTX) {
        DALI_LOGD("TX: %02x %02x", address, data);
        delayMicroseconds(BIT_PERIOD*8);
        //Serial.print("TX: "); Serial.print(address, HEX); Serial.print(" "); Serial.println(data, HEX);
    }

    {
        // This is timing critical
        InterruptLock lock;

        writeBit(1); // START bit
        writeByte(address);
        writeByte(data);
        m_txPin->digital_write(LOW);
    }

    // Non critical delay
    delayMicroseconds(HALF_BIT_PERIOD*2);
    delayMicroseconds(BIT_PERIOD*4); // Optional, for clarity in scope trace
}

uint8_t DaliBusComponent::receiveBackwardFrame(unsigned long timeout_ms) {
    uint8_t data;

    unsigned long startTime = millis();
    uint32_t startMicros = micros();

    // Wait for START bit (timing critical)
    // TODO: Need a better way to wait for this that doens't block the CPU
    while (m_rxPin->digital_read() == LOW) {
        if (millis() - startTime >= timeout_ms) {
            //Serial.println("No reply");
            return 0;
        }
    }

    {
        // This is timing critical
        InterruptLock lock;

        delayMicroseconds(BIT_PERIOD); // Wait for first data bit
        delayMicroseconds(QUARTER_BIT_PERIOD); // Wait a quater bit period to sample middle of first half bit
        data = readByte();
        delayMicroseconds(BIT_PERIOD*2); // Wait for STOP bits
    }

    //Serial.print("RX: "); Serial.println(data, HEX);
    if (DEBUG_LOG_RXTX) {
        DALI_LOGD("RX: %02x", data);
    }

    // Minimum time before we can send another forward frame
    delayMicroseconds(BIT_PERIOD*8); 
    return data;
}
