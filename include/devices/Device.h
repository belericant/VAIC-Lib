#ifndef INC_7405XV5_DEVICE_H
#define INC_7405XV5_DEVICE_H

#ifdef __cplusplus
#include "pros/apix.h"
#include "v5_api.h"
#include <any>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>
extern "C" {
#endif
void device_initialize();
#ifdef __cplusplus
}

namespace lib7405x::device {
    class Device;
    void register_device(Device* device);

    typedef struct {
        std::function<std::optional<double>(const std::any&, int)> read;
        std::any payload;
    } device_data_s_t;

    class Device {
        friend void lib7405x::device::register_device(Device* device);

    private:
        pros::c::v5_device_e_t _device_type;

    protected:
        explicit Device(pros::c::v5_device_e_t type) : _device_type(type) {}

    public:
        Device() : _device_type(pros::c::E_DEVICE_NONE) {}
        virtual void write(V5_DeviceT& device, const std::vector<int>& macros, const std::any& payload, uint8_t address) = 0;
        virtual device_data_s_t read(V5_DeviceT& device, uint8_t address) const = 0;
        static std::unordered_map<pros::c::v5_device_e_t, lib7405x::device::Device*, std::hash<int>> device_registration;
    };
}// namespace lib7405x::device
#endif//cplusplus
#endif//INC_7405XV5_DEVICE_H
