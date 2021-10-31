#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include "system/Components.h"
#include "system/Serial.h"

namespace lib7405x::display {
    void reloadConstants();
}

lib7405x::Components* lib7405x::Components::_instance = nullptr;

extern "C" void registry_init();

void components_initialize() {
    lib7405x::Components::Instance()->init();
}

void components_background_processing() {
    lib7405x::Components::Instance()->update();
}

lib7405x::Components* lib7405x::Components::Instance() {
    if (!_instance) {
        _instance = new Components;
    }
    return _instance;
}

void lib7405x::Components::init() {
    reloadConfig();
    updatePortBindings();
}

void lib7405x::Components::update() {
    while (!_deviceWrite.empty()) {
        //        sSerial->send(Serial::STDOUT, "dw: " + std::to_string(iter.second.address));
        const auto& iter = _deviceWrite.front();
        if (_portBindings.find(iter.first) != _portBindings.end()) {
            v5_smart_device_s_t* device = registry_get_device(_portBindings.at(iter.first));
            //            sSerial->send(Serial::STDOUT, "pb: " + iter.first);
            if (device::Device::device_registration.find(device->device_type) != device::Device::device_registration.end()) {
                //                                sSerial->send(Serial::STDOUT, iter.first);
                device::Device::device_registration.at(device->device_type)->write(device->device_info, iter.second.macros, iter.second.payload, iter.second.address);
            }
        }
        _deviceWrite.pop();
    }
    for (const auto& iter : _portBindings) {
        v5_smart_device_s_t* device = registry_get_device(iter.second);
        if (device::Device::device_registration.find(device->device_type) != device::Device::device_registration.end()) {
            if (_reverseSensorMap.find(iter.first) != _reverseSensorMap.end()) {
                for (const auto& i : _reverseSensorMap.at(iter.first)) {
                    //                    sSerial->send(Serial::STDOUT, "devread. Key: " + i.first +  " Value:  " + std::to_string(i.second));
                    _deviceRead[i.first] = device::Device::device_registration.at(device->device_type)->read(device->device_info, i.second);
                }
            } else {
                _deviceRead[iter.first] = device::Device::device_registration.at(device->device_type)->read(device->device_info, 0);
            }
        }
    }
}

void lib7405x::Components::write(const std::string& id, const std::vector<int>& macros, const std::any& payload) {
    if (_sensorMap.find(id) != _sensorMap.end()) {
        //        sSerial->send(Serial::STDOUT, "DM: " + _sensorMap.at(id).first + " : " + std::to_string(_sensorMap.at(id).second));
        _deviceWrite.emplace(_sensorMap.at(id).first, (data_s_t){.macros = macros, .payload = payload, .address = _sensorMap.at(id).second});
        //        {_sensorMap.at(id).first, (data_s_t){.macros = macros, .payload = payload, .address = _sensorMap.at(id).second}}
    } else {
        //        sSerial->send(Serial::STDOUT, "CLEARED BY: " + id );
        _deviceWrite.emplace(id, (data_s_t){.macros = macros, .payload = payload, .address = _portBindings.at(id)});
    }
}

void lib7405x::Components::updatePortBindings() {
    synchronized(this) {
        port_mutex_take_all();
        registry_init();
        uint32_t mtr_ctr = 0, sensor_ctr = 1;
        if (!_sensors.empty()) {
            _portBindings[_sensors.at(0)] = 22;
        }
        for (uint8_t i = 0; i < 22; ++i) {
            pros::c::v5_device_e_t type = registry_get_plugged_type(i);
            if (type == pros::c::E_DEVICE_MOTOR && mtr_ctr < _motors.size()) {
                _portBindings[_motors.at(mtr_ctr++)] = i;
            } else if (type != pros::c::E_DEVICE_NONE && type != pros::c::E_DEVICE_RADIO && sensor_ctr < _sensors.size()) {
                if (_sensors.at(sensor_ctr) != "native_adi") {
                    _portBindings[_sensors.at(sensor_ctr)] = i;
                }
                sensor_ctr++;
            }
        }
        port_mutex_give_all();
    }
    Serial::Instance()->updatePortBindings();
}

void lib7405x::Components::reloadConfig() {
    synchronized(this) {
        if (vexFileDriveStatus(0)) {
            FILE* file = fopen("/usd/config.json", "r");
            if (file) {
                // clear old data
                _motors.clear();
                _sensors.clear();
                _sensorMap.clear();
                _constants.clear();

                nlohmann::json prefs = nlohmann::json::parse(file);
                for (const auto& elem : prefs["motors"]) {
                    // loop through motors array
                    _motors.emplace_back(elem);
                }
                for (const auto& i : prefs["sensors"].get<nlohmann::json::object_t>()) {
                    // push smart device ids into vector
                    _sensors.push_back(i.first);
                    for (const auto& k : i.second.get<nlohmann::json::object_t>()) {
                        // add id to sensor lookup table
                        int portnum = std::stoi(k.second.get<std::string>());
                        if (portnum >= 0 && portnum <= 7) {
                            _sensorMap[k.first] = std::make_pair(i.first, portnum);
                            _reverseSensorMap[i.first].emplace_back(k.first, portnum);
                        }
                    }
                }
                for (const auto& elem : prefs["constants"].get<nlohmann::json::object_t>()) {
                    //loop through constants object, take the pair returned and insert it into map
                    _constants.emplace(elem.first, elem.second);
                }
                fclose(file);
            } else {
                Serial::Instance()->send(Serial::DISPLAY, "Config not found!");
                goto default_config;
            }
        } else {
        default_config:
            _constants["id"] = "0";
            _motors.emplace_back("BT_roller");
            _motors.emplace_back("FR_drive");
            _motors.emplace_back("FT_roller");
            _motors.emplace_back("BR_drive");
            _motors.emplace_back("R_intake");
            _motors.emplace_back("B_roller");
            _motors.emplace_back("L_intake");
            _motors.emplace_back("BL_drive");
            _motors.emplace_back("FL_drive");
        }
        display::reloadConstants();
    }
}

void lib7405x::Components::writeConfig() {
    synchronized(this) {
        if (vexFileDriveStatus(0)) {
            FILE* file = fopen("/usd/config.json", "w");
            nlohmann::json prefs;
            prefs["motors"] = _motors;
            for (const auto& iter : _sensorMap) {
                prefs["sensors"][iter.second.first].emplace(iter.first, iter.second.second);
            }
            prefs["constants"] = _constants;
            fputs(prefs.dump(4).c_str(), file);
            fclose(file);
        }
    }
}

std::optional<double> lib7405x::Components::read(const std::string& id, int macro) const {
    auto payload = read(id);
    if (payload.has_value()) {
        auto& data = payload.value();
        if (data.read) {
            return data.read(data.payload, macro);
        }
    }
    return std::nullopt;
}

std::optional<lib7405x::device::device_data_s_t> lib7405x::Components::read(const std::string& id) const {
    synchronized(this) {
        if (_deviceRead.find(id) != _deviceRead.end()) {
            return _deviceRead.at(id);
        }
    }
    return std::nullopt;
}

std::optional<std::string> lib7405x::Components::getConstant(const std::string& id) const {
    synchronized(this) {
        if (_constants.find(id) != _constants.end()) {
            return _constants.at(id);
        }
    }
    return std::nullopt;
}

void lib7405x::Components::updateConstant(const std::string& id, const std::string& value) {
    synchronized(this) {
        _constants.emplace(id, value);
        display::reloadConstants();
    }
}

std::unordered_map<std::string, std::string> lib7405x::Components::getConstants() {
    return _constants;
}
