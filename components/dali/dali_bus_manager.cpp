#include <Arduino.h>
#include "dali.h"

/// @brief Automatically assign sequential short addresses to all devices on the DALI bus
/// @param assign ASSIGN_ALL, ASSIGN_UNINITIALIZED, or the short address for a specific device
/// @return The number of devices found on the bus
uint8_t DaliBusManager::autoAssignShortAddresses(uint8_t assign, bool reset) {
    Serial.println();
    if (reset) {
        Serial.println("BEGIN AUTO ADDRESS ASSIGNMENT");
    } else {
        Serial.println("BEGIN AUTO ADDRESS QUERY");
    }

    // for (int i = 0; i < 64; i++) {
    //     m_addresses[i] = 0;
    // }

    // Put all devices on the bus into initialization mode, where they will accept special commands
    initialize(assign);

    // Tell all devices to randomize their addresses
    if (reset) {
        Serial.println("Randomizing addresses");
        randomize();
        delay(1000);
    }

    // Binary search through address space to find lowest address
    uint8_t count = 0;
    while (true) {
        uint32_t addr = 0x000000; // Start with the lowest address

        //NOTE: This doesn't work with one of my controllers!!
        // // Shortcut: test if we are done
        // if (!compareSearchAddress(0xFFFFFF)) {
        //     Serial.println("No more devices found");
        //     break;
        // }

        for (uint32_t i = 0; i < 24; i++) {
            uint32_t bit = 1ul << (uint32_t)(23ul - i);
            uint32_t search_addr = addr | bit;
            // Serial.print("Searching for addr 0x");
            // Serial.println(search_addr, HEX);

            // True if actual address <= search_address
            bool compare_result = compareSearchAddress(search_addr);
            if (compare_result) {
                addr &= ~bit; // Clear the bit (already clear)
            } else {
                addr |= bit;  // Set the bit
            }
        }

        if (addr == 0xFFFFFF) {
            break; // No more devices found
        }

        // Need to increment by one to get the actual address
        addr++;

        Serial.print("Found address: 0x"); Serial.println(addr, HEX);

        // Sanity check: Address should still return true for comparison
        if (!compareSearchAddress(addr)) {
            Serial.println("ERROR: Address did not match?");
            delay(5000);
            continue;
        }

        // Remove this device from the search
        withdraw(addr);

        // Sanity check: Address should no longer respond to comparison
        if (compareSearchAddress(addr)) {
            Serial.println("ERROR: Device did not withdraw");
            delay(5000);
            continue;
        }


        uint8_t short_addr = (count << 1);

        if (reset) {
            // Program short address
            // NOTE: an address of 0xFF will delete the short address
            port.sendSpecialCommand(
                DaliSpecialCommand::PROGRAM_SHORT_ADDRESS, 
                short_addr | DALI_COMMAND);
        }

        // Verify
        port.sendSpecialCommand(DaliSpecialCommand::VERIFY_SHORT_ADDRESS, short_addr | DALI_COMMAND);
        if (port.receiveBackwardFrame() == 0xFF) {
            Serial.print("Short address: "); Serial.println(short_addr, HEX);

            //m_addresses[count] = addr;
        } else {
            Serial.println("Short address verification failed!");
            delay(1000);
        }

        count++;
    }

    if (count == 0) {
        Serial.println("ERROR: No devices found");
    }

    // Exit initialization mode
    // Devices will respond to regular commands again
    terminate();

    //m_device_count = count;

    return count;
}



void DaliMaster::dumpStatusForDevice(uint8_t addr) {
    Serial.println();
    Serial.print("Device["); Serial.print(addr); Serial.println("]"); // Serial.print("] : 0x"); Serial.println(m_addresses[addr], HEX);
    bool present = port.sendQueryCommand(addr, DaliCommand::QUERY_CONTROL_GEAR_PRESENT);
    Serial.print("  Present:      "); Serial.println(present ? "YES" : "NO");

    if (!present) {
        return;
    }

    Serial.print("  Version:      "); Serial.println(port.sendQueryCommand(addr, DaliCommand::QUERY_VERSION_NUMBER), HEX);
    //Serial.print("  Device Type:  "); Serial.println(port.sendQueryCommand(addr, CMD_QUERY_DEVICE_TYPE), HEX); FF -> ALL?

    Serial.print("  Status:       "); //Serial.println(port.sendQueryCommand(addr, CMD_QUERY_STATUS), HEX);
        uint8_t status = port.sendQueryCommand(addr, DaliCommand::QUERY_STATUS);
        Serial.println(status, HEX);
        //if (status & STATUS_BALLAST_OK)             { Serial.println("    Ballast Not OK"); }
        if (status & STATUS_LAMP_FAILURE)           { Serial.println("    Lamp Failure"); }
        if (status & STATUS_LAMP_ON)                { Serial.println("    Lamp On"); } else { Serial.println("    Lamp Off"); }
        if (status & STATUS_LIMIT_ERROR)            { Serial.println("    Limit Error"); }
        if (status & STATUS_FADE_STATE)             { Serial.println("    Fading"); }
        //if (status & STATUS_RESET_STATE)            { Serial.println("    Reset State"); }
        if (status & STATUS_MISSING_SHORT_ADDRESS)  { Serial.println("    Missing Short Address"); }
        //if (status & STATUS_POWER_FAILURE)          { Serial.println("    Power Failure"); }

    Serial.print("  Op Mode:      "); Serial.println(port.sendQueryCommand(addr, DaliCommand::QUERY_OPERATING_MODE), HEX);
    
    Serial.print("  Lamp Failure: "); Serial.println(port.sendQueryCommand(addr, DaliCommand::QUERY_LAMP_FAILURE), HEX);
    Serial.print("  Lamp Power:   "); Serial.println(port.sendQueryCommand(addr, DaliCommand::QUERY_LAMP_POWER_ON) ? "ON" : "OFF");

    Serial.print("  Light Type:   "); //Serial.println(port.sendQueryCommand(addr, CMD_QUERY_LIGHT_SOURCE_TYPE), HEX);
        auto device_type = static_cast<DaliDeviceType>(port.sendQueryCommand(addr, DaliCommand::QUERY_LIGHT_SOURCE_TYPE));
        switch (device_type) {
            case DaliDeviceType::FLUORESCENT: Serial.println("Fluorescent"); break;
            case DaliDeviceType::EMERGENCY:    Serial.println("Emergency"); break;
            case DaliDeviceType::HID:          Serial.println("HID"); break;
            case DaliDeviceType::LV_HALOGEN:   Serial.println("Halogen"); break;
            case DaliDeviceType::INCANDESCENT: Serial.println("Incandescent"); break;
            case DaliDeviceType::DIGITAL:      Serial.println("Digital"); break;
            case DaliDeviceType::LED:          Serial.println("LED"); break;
            case DaliDeviceType::COLOR:        Serial.println("Colour"); break;
            default: Serial.println(static_cast<uint8_t>(device_type), HEX); break;
        }

        if (device_type == DaliDeviceType::LED) {
            Serial.print("    Extended Ver:   "); Serial.println(port.sendExtendedQuery(addr, DaliLedCommand::QUERY_EXTENDED_VERSION_NUMBER), HEX);
            Serial.print("    Features:       "); Serial.println(port.sendExtendedQuery(addr, DaliLedCommand::QUERY_FEATURES), HEX);
            Serial.print("    Dimming Curve:  "); Serial.println(port.sendExtendedQuery(addr, DaliLedCommand::QUERY_DIMMING_CURVE), HEX);
            Serial.print("    Type:           "); Serial.println(port.sendExtendedQuery(addr, DaliLedCommand::QUERY_GEAR_TYPE), HEX);
            Serial.print("    Failure:        "); Serial.println(port.sendExtendedQuery(addr, DaliLedCommand::QUERY_FAILURE_STATUS), HEX);
            Serial.print("    Operating Mode: "); Serial.println(port.sendExtendedQuery(addr, DaliLedCommand::QUERY_OPERATING_MODE), HEX);
            Serial.print("    Fast Fade Time: "); Serial.println(port.sendExtendedQuery(addr, DaliLedCommand::QUERY_FAST_FADE_TIME), HEX);

            uint8_t color_ver = port.sendExtendedQuery(addr, DaliColorCommand::QUERY_EXTENDED_VERSION_NUMBER);
            if (color_ver > 0) {
                Serial.println("    -- Colour --");
                Serial.print("    Extended Ver:   "); Serial.println(color_ver, HEX);
                uint8_t features = port.sendExtendedQuery(addr,DaliColorCommand::QUERY_COLOR_FEATURES);
                Serial.print("    Features:       "); Serial.println(features, HEX);
                    if (features & COLOR_FEATURE_XY_CAPABLE) { Serial.println("      XY Capable"); }
                    if (features & COLOR_FEATURE_TC_CAPABLE) { Serial.println("      TC Capable"); }
                Serial.print("    Status:         "); Serial.println(port.sendExtendedQuery(addr, DaliColorCommand::QUERY_COLOR_STATUS), HEX);
                
            }
        }

    Serial.print("  Actual Level: "); Serial.println(port.sendQueryCommand(addr, DaliCommand::QUERY_ACTUAL_LEVEL), HEX);
    Serial.print("  Max Level:    "); Serial.println(port.sendQueryCommand(addr, DaliCommand::QUERY_MAX_LEVEL), HEX);
    Serial.print("  Min Level:    "); Serial.println(port.sendQueryCommand(addr, DaliCommand::QUERY_MIN_LEVEL), HEX);
    Serial.print("  Power On Lvl: "); Serial.println(port.sendQueryCommand(addr, DaliCommand::QUERY_POWER_ON_LEVEL), HEX);

    Serial.print("  Fade Time/Rate: "); Serial.println(port.sendQueryCommand(addr, DaliCommand::QUERY_FADE_TIME_FADE_RATE), HEX);


}
