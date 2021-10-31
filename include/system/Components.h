#ifndef INC_7405XV5_COMPONENTS_H
#define INC_7405XV5_COMPONENTS_H

#ifdef __cplusplus
#include "devices/Device.h"
#include <map>
#include <string>
#include <unordered_map>
#include <queue>

extern "C" {
#include "vdml/registry.h"
}

extern "C" {
#endif
void components_initialize();
void components_background_processing();
#ifdef __cplusplus
}

namespace lib7405x {
    class Components {
        friend void ::components_initialize();
        friend void ::components_background_processing();

    private:
        Components() = default;
        Components(Components const&) = default;
        Components& operator=(Components const&) = default;
        static Components* _instance;
        inline Components* operator->() {
            return _instance;
        }

        typedef struct data_s {
            std::vector<int> macros;
            std::any payload;
            uint8_t address;
        } data_s_t;

        std::vector<std::string> _motors;
        std::vector<std::string> _sensors;
        std::unordered_map<std::string, std::pair<std::string, uint8_t>> _sensorMap;
        std::unordered_map<std::string, std::vector<std::pair<std::string, uint8_t>>> _reverseSensorMap;
        std::unordered_map<std::string, uint8_t> _portBindings;
        std::queue<std::pair<std::string, data_s_t>> _deviceWrite;
        std::unordered_map<std::string, device::device_data_s_t> _deviceRead;
        std::unordered_map<std::string, std::string> _constants;

        void init();
        void update();

    public:
        static Components* Instance();
        void write(const std::string& id, const std::vector<int>& macros, const std::any& payload);
        void updatePortBindings();
        void reloadConfig();
        void writeConfig();
        std::optional<double> read(const std::string& id, int macro) const;
        std::optional<device::device_data_s_t> read(const std::string& id) const;
        std::optional<std::string> getConstant(const std::string& id) const;
        void updateConstant(const std::string& id, const std::string& value);
        std::unordered_map<std::string, std::string> getConstants();
    };
}// namespace lib7405x
#endif
#endif//INC_7405XV5_COMPONENTS_H
