#pragma once
// https://ww1.microchip.com/downloads/en/AppNotes/90003200A.pdf
// https://github.com/arendst/Tasmota/blob/37f42b474f7116dcac40aeba218161303f486edb/tasmota/tasmota_xdrv_driver/xdrv_75_dali.ino
// https://github.com/sde1000/python-dali


#define DALI_COMMAND (0x01)
#define DALI_DIRECT_ARC (0x00)

#define ADDR_BROADCAST (0x7F)       // 1111 111S  (S: 1=command, 0=arc)
#define ADDR_GROUP     (0x40)       // 100x xxxS
#define ADDR_SPECIAL_COMMAND (0xA0) // 101x xxxS and 110x xxxS

#define CMD_OFF (0x00) // Switches off lamp(s)
#define CMD_UP (0x01) // Increases lamp(s) illumination level
#define CMD_DOWN (0x02) // Decreases lamp(s) illumination level
#define CMD_STEP_UP (0x03) // Increases the target illumination level by 1
#define CMD_STEP_DOWN (0x04) // Decreases the target illumination level by 1
#define CMD_RECALL_MAX_LEVEL (0x05) // Changes the current light output to the maximum level
#define CMD_RECALL_MIN_LEVEL (0x06) // Changes the current light output to the minimum level
#define CMD_STEP_DOWN_AND_OFF (0x07) // If the target level is zero, lamp(s) are turned off; if the target level is between the min. and max. levels, decrease the target level by one; if the target level is max., lamp(s) are turned off
#define CMD_ON_AND_STEP_UP (0x08) // If the target level is zero, lamp(s) are set to minimum level; if target level is between min. and max. levels, increase the target level by one
#define CMD_ENABLE_DAPC_SEQUENCE (0x09) // Indicates the start of DAPC (level) commands
#define CMD_GO_TO_LAST_ACTIVE_LEVEL (0x0A) // Sets the target level to the last active output level
#define CMD_CONTINUOUS_UP (0x0B) // Fade started until max level reached
#define CMD_CONTINUOUS_DOWN (0x0C) // Fade started until min level reached
#define CMD_GO_TO_SCENE (0x10) // Sets a group of lamps to a predefined scene // Bits 0-3: Scene number
#define CMD_DALI_RESET (0x20) // Configures all variables back to their Reset state
#define CMD_STORE_ACTUAL_LEVEL_IN_DTR0 (0x21) // Stores the actual level value into Data Transfer Register 0 (DTR0)
#define CMD_SAVE_PERSISTENT_VARIABLES (0x22) // Stores all variables into Nonvolatile Memory (NVM)
#define CMD_SET_OPERATING_MODE_DTR0 (0x23) // Sets the operating mode to the value listed in DTR0
#define CMD_RESET_MEMORY_BANK_DTR0 (0x24) // Resets the memory bank identified by DTR0 (memory bank must be implemented and unlocked)
#define CMD_IDENTIFY_DEVICE (0x25) // Instructs a control gear to run an identification procedure
#define CMD_SET_MAX_LEVEL_DTR0 (0x2A) // Configures the control gear's maximum output level to the value stored in DTR0
#define CMD_SET_MIN_LEVEL_DTR0 (0x2B) // Configures the control gear's minimum output level to the value stored in DTR0
#define CMD_SET_SYSTEM_FAILURE_LEVEL_DTR0 (0x2C) // Sets the control gear's output level in the event of a system failure to the value stored in DTR0
#define CMD_SET_POWER_ON_LEVEL_DTR0 (0x2D) // Configures the output level upon power-up based on the value of DTR0
#define CMD_SET_FADE_TIME_DTR0 (0x2E) // Sets the fade time based on the value of DTR0
#define CMD_SET_FADE_RATE_DTR0 (0x2F) // Sets the fade rate based on the value of DTR0
//#define CMD_SET_EXTENDED_FADE_TIME_DTR0 (0x30) // Sets the extended fade rate based on the value of DTR0; used when fade time = 0
#define CMD_SET_SCENE (0x40) // Configures scene 'x' based on the value of DTR0 // Bits 0-3: Scene number
#define CMD_REMOVE_FROM_SCENE (0x50) // Removes one of the control gears from a scene // Bits 0-3: Scene number
#define CMD_ADD_TO_GROUP (0x60) // Adds a control gear to a group // Bits 0-3: Group number
#define CMD_REMOVE_FROM_GROUP (0x70) // Removes a control gear from a group // Bits 0-3: Group number
#define CMD_SET_SHORT_ADDRESS_DTR0 (0x80) // Sets a control gear's short address to the value of DTR0
#define CMD_ENABLE_WRITE_MEMORY (0x81) // Allows writing into memory banks
#define CMD_QUERY_STATUS (0x90) // Determines the control gear's status based on a combination of gear properties
#define CMD_QUERY_CONTROL_GEAR_PRESENT (0x91) // Determines if a control gear is present
#define CMD_QUERY_LAMP_FAILURE (0x92) // Determines if a lamp has failed
#define CMD_QUERY_LAMP_POWER_ON (0x93) // Determines if a lamp is On
#define CMD_QUERY_LIMIT_ERROR (0x94) // Determines if the requested target level has been modified due to max. or min. level limitations
#define CMD_QUERY_RESET_STATE (0x95) // Determines if all NVM variables are in their Reset state
#define CMD_QUERY_MISSING_SHORT_ADDRESS (0x96) // Determines if a control gear's address is equal to 0xFF
#define CMD_QUERY_VERSION_NUMBER (0x97) // Returns the device's version number located in memory bank 0, location 0x16
#define CMD_QUERY_CONTENT_DTR0 (0x98) // Returns the value of DTR0
#define CMD_QUERY_DEVICE_TYPE (0x99) // Determines the device type supported by the control gear
#define CMD_QUERY_PHYSICAL_MINIMUM (0x9A) // Returns the minimum light output that the control gear can operate at
#define CMD_QUERY_POWER_FAILURE (0x9B) // Determines if an external power cycle occurred
#define CMD_QUERY_CONTENT_DTR1 (0x9C) // Returns the value of DTR1
#define CMD_QUERY_CONTENT_DTR2 (0x9D) // Returns the value of DTR2
#define CMD_QUERY_OPERATING_MODE (0x9E) // Determines the control gear's operating mode
#define CMD_QUERY_LIGHT_SOURCE_TYPE (0x9F) // Returns the control gear's type of light source
#define CMD_QUERY_ACTUAL_LEVEL (0xA0) // Returns the control gear's actual power output level
#define CMD_QUERY_MAX_LEVEL (0xA1) // Returns the control gear's maximum output setting
#define CMD_QUERY_MIN_LEVEL (0xA2) // Returns the control gear's minimum output setting
#define CMD_QUERY_POWER_ON_LEVEL (0xA3) // Returns the value of the intensity level upon power-up
#define CMD_QUERY_SYSTEM_FAILURE_LEVEL (0xA4) // Returns the value of the intensity level due to a system failure
#define CMD_QUERY_FADE_TIME_FADE_RATE (0xA5) // Returns a byte in which the upper nibble is equal to the fade time value and the lower nibble is the fade rate value
#define CMD_QUERY_MANUFACTURER_SPECIFIC_MODE (0xA6) // Returns a 'YES' when the operating mode is within the range of 0x80 - 0xFF
#define CMD_QUERY_NEXT_DEVICE_TYPE (0xA7) // Determines if the control gear has more than one feature, and if so, return the first/next device type or feature
#define CMD_QUERY_EXTENDED_FADE_TIME (0xA8) // Returns a byte in which bits 6-4 is the value of the extended fade time multiplier and the lower nibble is the extended fade time base
#define CMD_QUERY_CONTROL_GEAR_FAILURE (0xAA) // Determines if a control gear has failed
#define CMD_QUERY_SCENE_LEVEL (0xB0) // Returns the level value of scene 'x'
#define CMD_QUERY_GROUPS_0_7 (0xC0) // Returns a byte in which each bit represents a member of a group. A '1' represents a member of the group
#define CMD_QUERY_GROUPS_8_15 (0xC1) // Returns a byte in which each bit represents a member of a group. A '1' represents a member of the group
#define CMD_QUERY_RANDOM_ADDRESS_H (0xC2) // Returns the upper byte of a randomly generated address
#define CMD_QUERY_RANDOM_ADDRESS_M (0xC3) // Returns the high byte of a randomly generated address
#define CMD_QUERY_RANDOM_ADDRESS_L (0xC4) // Returns the low byte of a randomly generated address
#define CMD_READ_MEMORY_LOCATION (0xC5) // Returns the content of the memory location stored in DTR0 that is located within the memory bank listed in DTR1
//#define CMD_QUERY_FAST_FADE_TIME (0xFD) // Returns the fast fade time value
#define CMD_QUERY_EXTENDED_VERSION_NUMBER (0xFF) // Returns the version number belonging to the device type or feature
#define ECMD_QUERY_EXTENDED_VERSION_NUMBER (0xFF)

// Extended device commands (DEVICE_LIGHT_TYPE_LED)
// Must be preceded by command SCMD_ENABLE_DEVICE_TYPE
// https://github.com/sde1000/python-dali/blob/master/dali/gear/led.py
#define ECMD_LED_SELECT_DIMMING_CURVE (0xE3) // Selects the dimming curve (1=liner, 0=logarithmic)
#define ECMD_LED_STORE_DTR_AS_FAST_FADE_TIME (0xE4)
#define ECMD_LED_QUERY_GEAR_TYPE (0xED) 
#define ECMD_LED_QUERY_DIMMING_CURVE (0xEE) // See ECMD_SELECT_DIMMING_CURVE
#define ECMD_LED_QUERY_POSSIBLE_OPERATING_MODES (0xEF) // Enum
#define ECMD_LED_QUERY_FEATURES (0xF0) // Enum
#define ECMD_LED_QUERY_FAILURE_STATUS (0xF1) // Enum
#define ECMD_LED_QUERY_SHORT_CIRCUIT (0xF2) // Boolean
#define ECMD_LED_QUERY_OPEN_CIRCUIT (0xF3) // Boolean
#define ECMD_LED_QUERY_LOAD_DECREASE (0xF4) // Boolean
#define ECMD_LED_QUERY_LOAD_INCREASE (0xF5) // Boolean
#define ECMD_LED_QUERY_CURRENT_PROTECTOR_ACTIVE (0xF6) // Boolean
#define ECMD_LED_QUERY_THERMAL_SHUTDOWN (0xF7) // Boolean
#define ECMD_LED_QUERY_THERMAL_OVERLOAD (0xF8) // Boolean
#define ECMD_LED_QUERY_REFERENCE_MEASUREMENT_RUNNING (0xF9) // Boolean
#define ECMD_LED_QUERY_REFERENCE_MEASUREMENT_FAILED (0xFA) // Boolean
#define ECMD_LED_QUERY_CURRENT_PROTECTOR_ENABLED (0xFB) // Boolean
#define ECMD_LED_QUERY_OPERATING_MODE (0xFC) // Enum
#define ECMD_LED_QUERY_FAST_FADE_TIME (0xFD)
#define ECMD_LED_QUERY_MIN_FAST_FADE_TIME (0xFE)

// Extended device commands (DEVICE_LIGHT_TYPE_COLOR)
// https://github.com/sde1000/python-dali/blob/master/dali/gear/colour.py
// Tc: Color Temperature in Mirek
// RGBWAF : R, G, B, White, Amber, 'Free Color'
// XY : XY chromaticity
#define ECMD_COLOR_SET_X_COORD (224) // via DTR0 & DTR1
#define ECMD_COLOR_SET_Y_COORD (225) // via DTR0 & DTR1
#define ECMD_COLOR_ACTIVATE (226)
#define ECMD_COLOR_SET_TEMPERATURE (231)    // Via DTR0 & DTR1
#define ECMD_COLOR_TEMPERATURE_COOLER (232)
#define ECMD_COLOR_TEMPERATURE_WARMER (233)
#define ECMD_COLOR_SET_PRIMARY_NDIM_LEVEL (234) // Via DTR0,1,2
#define ECMD_COLOR_SET_RGB_DIM_LEVEL (235) // Via DTR0,1,2,3
#define ECMD_COLOR_SET_WAF_DIM_LEVEL (236) // Via DTR0,1,2,3
#define ECMD_COLOR_SET_RGBWAF_CONTROL (237) // Via DTR0
#define ECMD_COLOR_COPY_REPORT_TO_TEMPORARY (238)
#define ECMD_COLOR_STORE_TY_PRIMARY_N (240) // Via DTR0,1,2,3
#define ECMD_COLOR_STORE_XY_COORD_PRIMARY_N (241) // Via DTR2
#define ECMD_COLOR_ASSIGN_TO_LINKED_CHANNEL (245) // Via DTR0
#define ECMD_COLOR_START_AUTO_CALIBRATION (246)
#define ECMD_COLOR_QUERY_GEAR_FEATURES (247)
#define ECMD_COLOR_QUERY_COLOR_STATUS (248)
#define ECMD_COLOR_QUERY_COLOR_FEATURES (249)
#define ECMD_COLOR_QUERY_COLOR_VALUE (250)  // MSB in response, LSB in DTR0. Must call QueryActualLevel before calling this.
#define ECMD_COLOR_QUERY_RGBWAF_CONTROL (251)
#define ECMD_COLOR_QUERY_ASSIGNED_COLOR (252) // Depends on value of DTR0



// Special commands (command goes in address byte)

#define SCMD_TERMINATE (0xA1) // Stops the control gear's initialization
#define SCMD_DTR0_DATA (0xA3) // Loads a data byte into DTR0
#define SCMD_INITIALISE (0xA5) // Initializes a control gear, command must be issued twice
#define SCMD_RANDOMIZE (0xA7) // Generates a random address value, command must be issued twice
#define SCMD_COMPARE (0xA9) // Compares the random address variable to the search address variable
#define SCMD_WITHDRAW (0xAB) // Changes the initialization state to reflect that a control gear had been identified but remains in the initialization state
#define SCMD_PING (0xAD) // Used by control devices to indicate their presence on the bus
#define SCMD_SEARCH_ADDRH (0xB1) // Determines if an address is present on the bus
#define SCMD_SEARCH_ADDRM (0xB3) // Determines if an address is present on the bus
#define SCMD_SEARCH_ADDRL (0xB5) // Determines if an address is present on the bus
#define SCMD_PROGRAM_SHORT_ADDRESS (0xB7) // Programs a control gear's short address
#define SCMD_VERIFY_SHORT_ADDRESS (0xB9) // Verifies if a control gear's short address is correct
#define SCMD_QUERY_SHORT_ADDRESS (0xBB) // Queries a control gear's short address
#define SCMD_ENABLE_DEVICE_TYPE (0xC1) // Enables a control gear's device type function
#define SCMD_DTR1_DATA (0xC3) // Loads a data byte into DTR1
#define SCMD_DTR2_DATA (0xC5) // Loads a data byte into DTR2
#define SCMD_WRITE_MEMORY_LOCATION (0xC7) // Writes data into a specific memory location and returns the value of the data written
#define SCMD_WRITE_MEMORY_LOCATION_NO_REPLY (0xC9) // Writes data into a specific memory location but does not return a response


#define ASSIGN_ALL (0x00)
#define ASSIGN_UNINITIALIZED (0xFF)


#define STATUS_BALLAST_OK (0x01)
#define STATUS_LAMP_FAILURE (0x02)
#define STATUS_LAMP_ON (0x04)
#define STATUS_LIMIT_ERROR (0x08)
#define STATUS_FADE_STATE (0x10) // 0:ready, 1:fading
#define STATUS_RESET_STATE (0x20)
#define STATUS_MISSING_SHORT_ADDRESS (0x40)
#define STATUS_POWER_FAILURE (0x80)


// CMD_QUERY_LIGHT_SOURCE_TYPE
#define DEVICE_LIGHT_TYPE_FLURORESCENT (0)
#define DEVICE_LIGHT_TYPE_EMERGENCY (1)
#define DEVICE_LIGHT_TYPE_HID (2)
#define DEVICE_LIGHT_TYPE_LV_HALOGEN (3)
#define DEVICE_LIGHT_TYPE_INCANDESCENT (4)
#define DEVICE_LIGHT_TYPE_DIGITAL_CONVERSION (5) // DC dimmers?
#define DEVICE_LIGHT_TYPE_LED (6) // LED module
#define DEVICE_LIGHT_TYPE_COLOR (8)

// ECMD_QUERY_GEAR_TYPE
#define GEAR_TYPE_INTEGRATED_POWER_SUPPLY (0x00)
#define GEAR_TYPE_INTEGRATED_MODULE (0x01)
#define GEAR_TYPE_AC_SUPPLY (0x02)
#define GEAR_TYPE_DC_SUPPLY (0x03)

// ECMD_COLOR_QUERY_FEATURES
#define COLOR_FEATURE_XY_CAPABLE (0x01)
#define COLOR_FEATURE_TC_CAPABLE (0x02)
// ...

// ECMD_COLOR_QUERY_STATUS
#define COLOR_STATUS_XY_OUT_RANGE (0x01)
#define COLOR_STATUS_TC_OUT_RANGE (0x02)
#define COLOR_STATUS_AUTO_CALIB_RUNNING (0x04)
#define COLOR_STATUS_AUTO_CALIB_SUCCESSFUL (0x08)
#define COLOR_STATUS_XY_ACTIVE (0x10)
#define COLOR_STATUS_TC_ACTIVE (0x20)
#define COLOR_STATUS_RGBWAF_ACTIVE (0x40)

#define COLOR_QUERY_TC (2)
#define COLOR_QUERY_TC_COOLEST (128)
#define COLOR_QUERY_TC_WARMEST (130)
#define COLOR_QUERY_REPORT_TC (226)
#define COLOR_QUERY_COLOR_TYPE (240)


#define MIREK_COOLEST (10)
#define MIREK_WARMEST (1000)
