#ifndef INC_7405XV5_COLOR_H
#define INC_7405XV5_COLOR_H

#include "Device.h"

namespace lib7405x::device {
    class Color : public Device {
    public:
        typedef struct data_s {
            double hue, saturation, brightness, proximity;
        } data_s_t;

        typedef enum color_e {
            HUE,
            SATURATION,
            BRIGHTNESS,
            PROXIMITY,
            LED
        } color_e_t;

        Color();
        void write(V5_DeviceT& device, const std::vector<int>& macros, const std::any& payload, uint8_t address) override;
        device_data_s_t read(V5_DeviceT& device, uint8_t address) const override;
        static std::optional<double> get(const std::any& payload, int macro);
    };
}// namespace lib7405x::device

#endif//INC_7405XV5_COLOR_H
