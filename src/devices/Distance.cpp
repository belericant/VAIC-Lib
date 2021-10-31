#include "devices/Distance.h"
#include <v5_api.h>
#include <vector>

lib7405x::device::Distance::Distance() : Device(pros::c::E_DEVICE_DISTANCE) {}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void lib7405x::device::Distance::write(V5_DeviceT& device, const std::vector<int>& macros, const std::any& payload, uint8_t address) {
    // stub
}

lib7405x::device::device_data_s_t lib7405x::device::Distance::read(V5_DeviceT& device, uint8_t address) const {
    return device_data_s_t{.read = get, .payload = data_s_t{.distance = (double) vexDeviceDistanceDistanceGet(device), .confidence = (double) vexDeviceDistanceConfidenceGet(device)}};
}
#pragma GCC diagnostic pop

std::optional<double> lib7405x::device::Distance::get(const std::any& payload, int macro) {
    try {
        auto data = std::any_cast<data_s_t>(payload);
        switch (macro) {
            case DISTANCE:
                return data.distance;

            case CONFIDENCE:
                return data.confidence;
            default:
                return std::nullopt;
        }
    } catch (std::bad_any_cast& e) {
        return std::nullopt;
    }
}
