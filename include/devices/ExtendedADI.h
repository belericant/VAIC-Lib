#ifndef INC_7405XV5_EXTENDEDADI_H
#define INC_7405XV5_EXTENDEDADI_H

#include "Device.h"

namespace lib7405x::device {
    class ExtendedADI : public Device {
    public:
        typedef struct data_s {
            double value;
            bool reset;
        } data_s_t;

        typedef enum adi_e {
            ENCODER,
            DIGITAL,
            ANALOG
        } adi_e_t;

        ExtendedADI();
        void write(V5_DeviceT& device, const std::vector<int>& macros, const std::any& payload, uint8_t address) override;
        device_data_s_t read(V5_DeviceT& device, uint8_t address) const override;
        static std::optional<double> get(const std::any& payload, int macro);
    };
}// namespace lib7405x::device

#endif//INC_7405XV5_EXTENDEDADI_H
