#include "devices/ExtendedADI.h"
#include "devices/Device.h"
#include "system/Serial.h"
#include <v5_api.h>
#include <vdml/registry.h>
lib7405x::device::ExtendedADI::ExtendedADI() : Device(pros::c::E_DEVICE_ADI) {}

void lib7405x::device::ExtendedADI::write(V5_DeviceT& device, const std::vector<int>& macros, const std::any& payload, uint8_t address) {
    try {
        auto data = std::any_cast<ExtendedADI::data_s_t>(payload);
        for (const auto& macro : macros) {
            switch (macro) {
                case ENCODER:
                    if (address % 2 == 0) {
                        vexDeviceAdiPortConfigSet(device, address, kAdiPortTypeQuadEncoder);
                        if (data.reset) {
                            vexDeviceAdiValueSet(device, address, 0);
                        }
                    }
                    break;

                case DIGITAL:
                    vexDeviceAdiPortConfigSet(device, address, kAdiPortTypeDigitalIn);
                    break;

                case ANALOG:
                    vexDeviceAdiPortConfigSet(device, address, kAdiPortTypeAnalogIn);
                    break;

                default:
                    break;
            }
        }
    } catch (std::bad_any_cast& e) {
        return;
    }
}

lib7405x::device::device_data_s_t lib7405x::device::ExtendedADI::read(V5_DeviceT& device, uint8_t address) const {
    return device_data_s_t{.read = get, .payload = data_s_t{.value = static_cast<double>(vexDeviceAdiValueGet(device, address)), .reset = false}};
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
std::optional<double> lib7405x::device::ExtendedADI::get(const std::any& payload, int macro) {
    try {
        auto data = std::any_cast<ExtendedADI::data_s_t>(payload);
        return data.value;
    } catch (std::bad_any_cast& e) {
        return std::nullopt;
    }
}
#pragma GCC diagnostic pop