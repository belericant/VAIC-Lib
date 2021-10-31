#ifndef INC_7405XV5_DISTANCE_H
#define INC_7405XV5_DISTANCE_H

#include "Device.h"

namespace lib7405x::device {
    class Distance : public Device {
    public:
        typedef struct data_s {
            double distance, confidence;
        } data_s_t;

        typedef enum distance_e {
            DISTANCE,
            CONFIDENCE
        } distance_e_t;

        Distance();
        void write(V5_DeviceT& device, const std::vector<int>& macros, const std::any& payload, uint8_t address) override;
        device_data_s_t read(V5_DeviceT& device, uint8_t address) const override;
        static std::optional<double> get(const std::any& payload, int macro);
    };

}// namespace lib7405x::device
#endif//INC_7405XV5_DISTANCE_H
