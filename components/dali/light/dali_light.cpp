#include "esphome.h"
#include "dali_light.h"

using namespace esphome;
using namespace dali;

using namespace esphome::light;

static const char *const TAG = "dali.light";

#define DALI_MAX_BRIGHTNESS_F (254.0f)

void dali::DaliLight::setup_state(light::LightState *state) {
    // Initialization code for DaliLight
    ESP_LOGD(TAG, "DALI SETUP");

    // Exclude broadcast and group addresses
    if ((this->address_ != ADDR_BROADCAST) && ((this->address_ & ADDR_GROUP_MASK) == 0)) {
        ESP_LOGD(TAG, "Querying DALI device capabilities...");
        if (bus->dali.isDevicePresent(address_)) {

            // NOTE: Some DALI controllers report their device type is LED(6) even though they do also support color temperature,
            // so let's explicitly check if they respond to this:
            this->tc_supported_ = bus->dali.color.isTcCapable(address_);
            if (tc_supported_) {
                ESP_LOGD(TAG, "Supports color temperature");

                // TODO: Don't seem to be getting the full range here?
                // Tc(cool)=153, Tc(warm)=370
                uint16_t coolest = bus->dali.color.queryParameter(address_, DaliColorParam::ColourTemperatureTcCoolest);
                uint16_t warmest = bus->dali.color.queryParameter(address_, DaliColorParam::ColourTemperatureTcWarmest);

                ESP_LOGD(TAG, "Tc(cool)=%d, Tc(warm)=%d", coolest, warmest);

                // Store reported coolest/warmest mired values for mapping.
                // NOTE: Not updating the configuration-provided warm/cool values, those are for UI only.
                // Ultimately we don't really want to trust the mired range reported by the dimmer
                // as it depends on the LED strip attached. So we map the UI range into the reported range.
                this->dali_tc_coolest_ = (float)coolest;
                //this->dali_tc_warmest_ = (float)warmest;
            }
            else {
                ESP_LOGD(TAG, "Does not support color temperature");
            }

            // if (state->initial_state_.has_value() && state->initial_state_.value().color_mode != ColorMode::UNKNOWN) {
            //     this->tc_supported_ = state->initial_state_.value().color_mode == ColorMode::COLOR_TEMPERATURE;
            //     if (tc_supported_) {
            //         ESP_LOGD(TAG, "Override: supports color temperature");
            //     } else {
            //         ESP_LOGD(TAG, "Override: does not support color temperature");
            //     }
            // }

            // // Optional configuration:
            // dali.lamp.setFadeRate(0, 7);
            // dali.lamp.setFadeTime(0, 3);
            // dali.lamp.setPowerOnLevel(0, 10);
            // dali.lamp.setMinLevel(0, 50);
            // dali.led.setDimmingCurve(0, DaliLedDimmingCurve::Linear);

            // if (this->initial_state_.has_value()) {
            //     bool power_on_state = (this->initial_state_->state);
            //     dali.lamp.setPowerOnLevel(0, 10);
            // }
        }
        else {
            ESP_LOGW(TAG, "DALI device at addr %x not found!", address_);
        }

        //bus->dali.dumpStatusForDevice(address_);
    }
    else {
        // TODO: How do we detect color temperature support for broadcast and group addresses?
    }
}

light::LightTraits dali::DaliLight::get_traits() {
    light::LightTraits traits;

    // NOTE: This is called repeatedly, do not perform any bus queries here...

    if (this->tc_supported_) {
        traits.set_supported_color_modes({light::ColorMode::COLOR_TEMPERATURE});
        traits.set_min_mireds(this->cold_white_temperature_);
        traits.set_max_mireds(this->warm_white_temperature_);
    }
    else {
        traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS});
    }

    return traits;
}

void dali::DaliLight::write_state(light::LightState *state) {
    bool on;
    float brightness;
    float color_temperature;

    state->current_values_as_binary(&on);
    if (!on) {
        // Short cut: send power off command
        bus->dali.lamp.turnOff(address_);
        return;
    }

    if (tc_supported_) {
        state->current_values_as_ct(&color_temperature, &brightness);

        // Map temperature 0..1 to reported TC coolest/warmest mireds
        // NOTE: Not using the configuration warm/cool colours - these may not match the reported range of the DALI device.
        float color_temperature_mired = (color_temperature * (dali_tc_warmest_ - dali_tc_coolest_)) + dali_tc_coolest_;

        //float color_temperature_mired = (color_temperature * (warm_white_temperature_ - cold_white_temperature_)) + cold_white_temperature_;
        //float color_temperature_mired = (color_temperature * (1000 - 10)) + 10;
        
        uint16_t dali_color_temperature = static_cast<uint16_t>(color_temperature_mired);

        //ESP_LOGD(TAG, "Tc=%f (%d)", color_temperature_mired, dali_color_temperature);

        // IMPORTANT: Do not set start_fade (activate), or the color temperature fade will
        // be cancelled when we next call setBrightness, and no color change will occur.
        bus->dali.color.setColorTemperature(address_, dali_color_temperature, false);
    } else {
        state->current_values_as_brightness(&brightness);
    }

    uint8_t dali_brightness = static_cast<uint8_t>(brightness * DALI_MAX_BRIGHTNESS_F);

    //ESP_LOGD(TAG, "B=%.2f (%d)", brightness, dali_brightness);
    bus->dali.lamp.setBrightness(address_, dali_brightness);
}
