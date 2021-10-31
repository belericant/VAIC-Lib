#include "devices/Color.h"
#include <v5_api.h>
#include <vector>

lib7405x::device::Color::Color() : Device(pros::c::E_DEVICE_OPTICAL) {}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void lib7405x::device::Color::write(V5_DeviceT& device, const std::vector<int>& macros, const std::any& payload, uint8_t address) {
    try {
        auto data = std::any_cast<data_s_t>(payload);
        for (const auto& macro : macros) {
            switch (macro) {
                case LED:
                    vexDeviceOpticalLedPwmSet(device, data.brightness);
                    break;

                default:
                    break;
            }
        }
    } catch (std::bad_any_cast& e) {
        return;
    }
}

lib7405x::device::device_data_s_t lib7405x::device::Color::read(V5_DeviceT& device, uint8_t address) const {
    return device_data_s_t{.read = get, .payload = data_s_t{.hue = vexDeviceOpticalHueGet(device), .saturation = vexDeviceOpticalSatGet(device), .brightness = vexDeviceOpticalBrightnessGet(device), .proximity = (double) vexDeviceOpticalProximityGet(device)}};
}
#pragma GCC diagnostic pop

std::optional<double> lib7405x::device::Color::get(const std::any& payload, int macro) {
    try {
        auto data = std::any_cast<data_s_t>(payload);
        switch (macro) {
            case HUE:
                return data.hue;

            case SATURATION:
                return data.saturation;

            case BRIGHTNESS:
                return data.brightness;

            case PROXIMITY:
                return data.proximity;
            default:
                return std::nullopt;
        }
    } catch (std::bad_any_cast& e) {
        return std::nullopt;
    }
}
