#include "devices/safe_vexlink.h"
#include "system/Components.h"
#include "system/Serial.h"
#include "system/Tasks.h"
#include "system/display.h"


static task_stack_t serial_task_stack[TASK_STACK_DEPTH_DEFAULT];
static static_task_s_t serial_task_buffer;
static void* serial_task_handle;

static task_stack_t serial_read_task_stack[TASK_STACK_DEPTH_DEFAULT];
static static_task_s_t serial_read_task_buffer;
static void* serial_read_task_handle;

static task_stack_t serial_link_task_stack[TASK_STACK_DEPTH_DEFAULT];
static static_task_s_t serial_link_task_buffer;
static void* serial_link_task_handle;

extern "C" void registry_init();
extern "C" int32_t inp_buffer_available();

static int getRadioPort() {
    for (uint8_t i = 0; i < 21; ++i) {
        pros::c::v5_device_e_t type = registry_get_plugged_type(i);
        if (type == E_DEVICE_RADIO) {
            return i;
        }
    }
    return -1;
}

__attribute__((constructor(102))) static lib7405x::device::safe_vexlink* get_wireless_link(bool forceRefresh = false) {
    static FILE* linkFile = fopen("/usd/linkID.txt", "r");
    static char id = linkFile ? (char) fgetc(linkFile) : '0';
    static int port = getRadioPort();
    static std::unique_ptr<lib7405x::device::safe_vexlink> wireless_link = port == -1 ? std::make_unique<lib7405x::device::safe_vexlink>() : std::make_unique<lib7405x::device::safe_vexlink>(port, lib7405x::Serial::LINK_ID, id == '0' ? vex::linkType::worker : vex::linkType::manager, false);
    static pros::mutex_t _mutex = pros::c::mutex_create();
    if (forceRefresh && pros::c::mutex_take(_mutex, 2)) {
        linkFile = fopen("/usd/linkID.txt", "r");
        id = linkFile ? (char) fgetc(linkFile) : '0';
        port = getRadioPort();
        wireless_link = port == -1 ? std::make_unique<lib7405x::device::safe_vexlink>() : std::make_unique<lib7405x::device::safe_vexlink>(port, lib7405x::Serial::LINK_ID, id == '0' ? vex::linkType::worker : vex::linkType::manager, false);
        auto ret = wireless_link.get();
        pros::c::mutex_give(_mutex);
        return ret;
    }
    if (!pros::c::mutex_take(_mutex, TIMEOUT_MAX)) {
        return nullptr;
    }
    auto ret = wireless_link.get();
    pros::c::mutex_give(_mutex);
    return ret;
}

lib7405x::Serial* lib7405x::Serial::_instance = nullptr;

lib7405x::Serial* lib7405x::Serial::Instance() {
    if (!_instance) {
        _instance = new Serial;
    }
    return _instance;
}

void serial_initialize() {
    serial_task_handle = task_create_static(serial_background_processing, nullptr, TASK_PRIORITY_DEFAULT + 1, TASK_STACK_DEPTH_DEFAULT, "lib Serial Daemon", serial_task_stack, &serial_task_buffer);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void serial_background_processing(void* ign) {
    lib7405x::Serial::Instance()->process();
}
#pragma GCC diagnostic pop

lib7405x::Serial::Serial() {
    serial_read_task_handle = task_create_static(Serial::internal_read, nullptr, TASK_PRIORITY_DEFAULT + 1, TASK_STACK_DEPTH_DEFAULT, "STDIN Read Daemon", serial_read_task_stack, &serial_read_task_buffer);
    serial_link_task_handle = task_create_static(Serial::vexlink_read, nullptr, TASK_PRIORITY_DEFAULT + 1, TASK_STACK_DEPTH_DEFAULT, "VEXLINK Read Daemon", serial_link_task_stack, &serial_link_task_buffer);
    Guard::_mutexes[(uintptr_t) _write_mutex] = _write_mutex;
    Guard::_mutexes[(uintptr_t) _callback_mutex] = _callback_mutex;
}

[[noreturn]] void lib7405x::Serial::process() {
    uint32_t time = pros::c::millis();
    while (true) {
        if (!pros::c::mutex_take(_write_mutex, 2)) {
            continue;
        }
        nlohmann::json sout_pkt;
        nlohmann::json serr_pkt;
        while (!_outbound.empty()) {
            message_s_t msg = _outbound.front();
            switch (msg.stream) {
                case STDOUT:
                    sout_pkt[msg.header] = msg.msg;
                    break;
                case STDERR:
                    serr_pkt[msg.header] = msg.msg;
                    break;
                case DISPLAY:
                case VEXLINK:
                    internal_write(msg);
                    break;
            }
            _outbound.pop();
        }
        if (!sout_pkt.is_null()) {
            internal_write({"v5 SOUT", sout_pkt.dump(), STDOUT, pros::c::millis()});
        }
        if (!serr_pkt.is_null()) {
            internal_write({"v5 SERR", serr_pkt.dump(), STDERR, pros::c::millis()});
        }

        pros::c::mutex_give(_write_mutex);
        pros::c::task_delay_until(&time, 5);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
[[noreturn]] void lib7405x::Serial::vexlink_read(void* ign) {
    static uint32_t MAX_SIZE = 255, CURRENT_SIZE = 0;
    static char* buf = static_cast<char*>(malloc(MAX_SIZE));
    uint32_t time = pros::c::millis();
    while (true) {
        uint8_t payload[1000];
        if (get_wireless_link()->receive(payload, 1000, 10) > 0) {
            for (int i = 0; i < 1000; ++i) {
                char c = (char) payload[i];
                if (c == '\002') {
                    continue;
                }
                if (CURRENT_SIZE == MAX_SIZE - 1) {
                    buf = static_cast<char*>(realloc(buf, CURRENT_SIZE + MAX_SIZE));
                    MAX_SIZE += CURRENT_SIZE;
                }
                if (c == '\n') {
                    buf[CURRENT_SIZE] = '\0';
                    std::string str(strdup(buf));
                    free(buf);
                    MAX_SIZE = 255;
                    CURRENT_SIZE = 0;
                    buf = static_cast<char*>(malloc(MAX_SIZE));
                    if (!str.empty()) {
                        auto header = str.substr(0, str.find(DELIMITER));
                        auto msg = str.substr(str.find(DELIMITER) + 1, str.size());
                        display::Terminal::serial->println(std::string("[Time ").append(std::to_string(pros::c::millis() / 1000)).append(", VEXLINK IN]: ").append(header).append(std::string(" ")).append(msg));
                        if (pros::c::mutex_take(Serial::Instance()->_callback_mutex, 2)) {
                            if (Serial::Instance()->_callbacks.find(header) != Serial::Instance()->_callbacks.end()) {
                                Serial::Instance()->_callbacks.at(header)(nlohmann::json::parse(msg));
                            }
                            pros::c::mutex_give(Serial::Instance()->_callback_mutex);
                        }
                    }
                    while ((c = (char) payload[i]) != '\002') {
                        if (++i == 1000) {
                            break;
                        }
                    }
                } else {
                    buf[CURRENT_SIZE++] = c;
                }
            }
        }
        pros::c::task_delay_until(&time, 15);
    }
}

[[noreturn]] void lib7405x::Serial::internal_read(void* ign) {
    uint32_t MAX_SIZE = 255, CURRENT_SIZE = 0, retval;
    char* buf = static_cast<char*>(malloc(MAX_SIZE));
    uint32_t time = pros::c::millis();
    while (true) {
        retval = inp_buffer_available();
        if (retval > 0) {
            int c;
            while ((c = getc(stdin)) != EOF && c != '\n') {
                if (CURRENT_SIZE == MAX_SIZE - 1) {
                    buf = static_cast<char*>(realloc(buf, CURRENT_SIZE + MAX_SIZE));
                    MAX_SIZE += CURRENT_SIZE;
                }
                buf[CURRENT_SIZE++] = c;
            }
            if (c == '\n') {
                buf[CURRENT_SIZE] = '\0';
                std::string str(strdup(buf));
                CURRENT_SIZE = 0;
                if (!str.empty()) {
                    auto header = str.substr(0, str.find(DELIMITER));
                    auto msg = str.substr(str.find(DELIMITER) + 1, str.size());
                    display::Terminal::serial->println(std::string("[Time ").append(std::to_string(pros::c::millis() / 1000)).append(", STDIN]: ").append(header).append(std::string(" ")).append(msg));
                    if (pros::c::mutex_take(Serial::Instance()->_callback_mutex, 2)) {
                        if (Serial::Instance()->_callbacks.find(header) != Serial::Instance()->_callbacks.end()) {
                            auto tempstr = nlohmann::json(msg).get<std::string>();
                            if (tempstr[tempstr.length()] == 'r') {// if there is a return character (depending on your serial settings I guess)
                                Serial::Instance()->_callbacks.at(header)(nlohmann::json::parse(tempstr.substr(0, tempstr.length() - 1)));
                            }
                            Serial::Instance()->_callbacks.at(header)(nlohmann::json(nlohmann::json::parse(tempstr)));
                        }
                        pros::c::mutex_give(Serial::Instance()->_callback_mutex);
                    }
                }
            }
        }
        pros::c::task_delay_until(&time, 5);
    }
}
#pragma GCC diagnostic pop

void lib7405x::Serial::internal_write(const lib7405x::Serial::message_s_t& msg) {
    std::string str = msg.header + DELIMITER + msg.msg;
    std::string wireless_str = "\002" + str + "\n";

    switch (msg.stream) {
        case STDOUT:
            std::cout << str << std::endl;
            break;

        case STDERR:
            std::cerr << str << std::endl;
            break;

        case VEXLINK:
            display::Terminal::serial->println(std::string("[Time ").append(std::to_string(pros::c::millis() / 1000)).append(", VEXLINK OUT]: ").append(msg.header).append(std::string(" ")).append(msg.msg));
            get_wireless_link()->send(strdup(wireless_str.c_str()), wireless_str.size());
            break;

        case DISPLAY:
            display::Terminal::logs->println(std::string("[Time ").append(std::to_string(pros::c::millis() / 1000)).append("]: ").append(msg.header));

        default:
            break;
    }
}

void lib7405x::Serial::send(ser_e_t stream, const std::string& header, const nlohmann::json& msg) {
    synchronized(_write_mutex) {
        _outbound.push(message_s_t{header, msg.dump(-1), stream, pros::c::millis()});
    }
}

void lib7405x::Serial::onReceive(const std::string& header, const std::function<void(nlohmann::json)>& callback) {
    synchronized(_callback_mutex) {
        _callbacks[header] = callback;
    }
}

void lib7405x::Serial::deregisterCallback(const std::string& header) {
    synchronized(_callback_mutex) {
        if (_callbacks.find(header) != _callbacks.end()) {
            _callbacks.erase(header);
        }
    }
}

bool lib7405x::Serial::hasWirelessLink() {
    return get_wireless_link()->isLinked();
}

void lib7405x::Serial::updatePortBindings() {
    get_wireless_link(true);
}
