#pragma once

#include <esphome.h>
#include "dali.h"

namespace esphome {
namespace dali {

class DaliBusComponent : public Component, public DaliPort {
public:
    DaliBusComponent()
        : Component { }
        , dali { *this }
    { }

    void setup() override;
    void loop() override;
    void dump_config() override;

    void set_tx_pin(GPIOPin* tx_pin) { m_txPin = tx_pin; }
    void set_rx_pin(GPIOPin* rx_pin) { m_rxPin = rx_pin; }

    void do_device_discovery() { m_discovery = true; }
    void do_initialize_addresses() { m_initialize_addresses = true; }

    DaliMaster dali;

public: // DaliPort
    void sendForwardFrame(uint8_t address, uint8_t data) override;
    uint8_t receiveBackwardFrame(unsigned long timeout_ms = 100) override;

private:
    void writeBit(bool bit);
    void writeByte(uint8_t b);
    uint8_t readByte();

    void create_light_component(short_addr_t short_addr, uint32_t long_addr);

    GPIOPin* m_rxPin;
    GPIOPin* m_txPin;

    bool m_discovery = false;
    bool m_initialize_addresses = false;
    uint32_t m_discovered_addresses[64];
};

}  // namespace dali
}  // namespace esphome
