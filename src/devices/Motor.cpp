#include "devices/Motor.h"
#include <v5_api.h>
#include <vector>
#include "system/Serial.h"
lib7405x::device::Motor::Motor() : Device(pros::c::E_DEVICE_MOTOR) {}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void lib7405x::device::Motor::write(V5_DeviceT& device, const std::vector<int>& macros, const std::any& payload, uint8_t address) {
    try {
        auto data = std::any_cast<data_s_t>(payload);
        for (const auto& macro : macros) {
            switch (macro) {
                case VOLTAGE:
                    vexDeviceMotorVoltageSet(device, data.voltage);
                    break;

                case POSITION:
                    vexDeviceMotorPositionSet(device, data.position);
                    break;

                case VELOCITY:
                    vexDeviceMotorVelocitySet(device, data.velocity);
                    break;

                case BRAKE_TYPE:
                    vexDeviceMotorBrakeModeSet(device, data.brakeMode);
                    break;

                default:
                    break;
            }
        }
    } catch (std::bad_any_cast& e) {
        return;
    }
}

lib7405x::device::device_data_s_t lib7405x::device::Motor::read(V5_DeviceT& device, uint8_t address) const {
    return device_data_s_t{.read = get, .payload = data_s_t{.voltage = vexDeviceMotorVoltageGet(device), .velocity = vexDeviceMotorVelocityGet(device), .position = vexDeviceMotorPositionGet(device), .brakeMode = vexDeviceMotorBrakeModeGet(device)}};
}
#pragma GCC diagnostic pop

std::optional<double> lib7405x::device::Motor::get(const std::any& payload, int macro) {
    try {
        auto data = std::any_cast<data_s_t>(payload);
        switch (macro) {
            case VOLTAGE:
                return data.voltage;

            case POSITION:
                return data.position;

            case VELOCITY:
                return data.velocity;

            case BRAKE_TYPE:
                return data.brakeMode;

            default:
                return std::nullopt;
        }
    } catch (std::bad_any_cast& e) {
        return std::nullopt;
    }
}