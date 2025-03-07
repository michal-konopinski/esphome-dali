#include "esphome_dali.h"
#include "esphome.h"
#include "dali.h"

static const char *const TAG = "dali";
static const bool DEBUG_LOG_RXTX = false;

using namespace esphome;
using namespace dali;

void DaliBusComponent::setup() {
    m_txPin->pin_mode(gpio::Flags::FLAG_OUTPUT);
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
        ESP_LOGD(TAG, "TX: %02x %02x", address, data);
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
        ESP_LOGD(TAG, "RX: %02x", data);
    }

    // Minimum time before we can send another forward frame
    delayMicroseconds(BIT_PERIOD*8); 
    return data;
}
