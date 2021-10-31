#ifndef INC_7405XV5_MOTOR_H
#define INC_7405XV5_MOTOR_H

#include "Device.h"

namespace lib7405x::device {
    class Motor : public Device {
    public:
        typedef struct data_s {
            int32_t voltage, velocity;
            double position;
            V5MotorBrakeMode brakeMode;
        } data_s_t;

        typedef enum motor_e {
            VOLTAGE,
            POSITION,
            VELOCITY,
            BRAKE_TYPE
        } motor_e_t;

        Motor();
        void write(V5_DeviceT& device, const std::vector<int>& macros, const std::any& payload, uint8_t address) override;
        device_data_s_t read(V5_DeviceT& device, uint8_t address) const override;
        static std::optional<double> get(const std::any& payload, int macro);
    };
}// namespace lib7405x::device

#endif//INC_7405XV5_MOTOR_H