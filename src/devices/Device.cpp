#include "devices/Device.h"
#include "devices/ExtendedADI.h"
#include "devices/Motor.h"
#include "devices/Color.h"
#include "devices/Distance.h"
std::unordered_map<pros::c::v5_device_e_t, lib7405x::device::Device*, std::hash<int>> __attribute__((init_priority(101))) lib7405x::device::Device::device_registration = std::unordered_map<pros::c::v5_device_e_t, lib7405x::device::Device*, std::hash<int>>();

void lib7405x::device::register_device(Device* device) {
    Device::device_registration.emplace(device->_device_type, device);
}

void device_initialize() {
    lib7405x::device::register_device(new lib7405x::device::Motor);
    lib7405x::device::register_device(new lib7405x::device::ExtendedADI);
    lib7405x::device::register_device(new lib7405x::device::Color);
    lib7405x::device::register_device(new lib7405x::device::Distance);
}