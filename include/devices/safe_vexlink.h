#ifndef INC_7405XV5_SAFE_VEXLINK_H
#define INC_7405XV5_SAFE_VEXLINK_H

#include "kapi.h"
#include "vex/v5_cpp.h"
#include "vex/vex_vexlink.h"

#include <memory>

namespace lib7405x::device {
    class safe_vexlink {
    private:
        std::unique_ptr<vex::serial_link> _internal_link = nullptr;
        const pros::mutex_t _mutex = pros::c::mutex_create();

    public:
        safe_vexlink() = default;
        safe_vexlink(int32_t index, const char* name, vex::linkType type, bool isWired = false) : _internal_link(std::make_unique<vex::serial_link>(index, name, type, isWired)) {}
        ~safe_vexlink() {
            pros::c::mutex_delete(_mutex);
        }

        bool isLinked() {
            if (!_internal_link || !pros::c::mutex_take(_mutex, 2)) {
                return false;
            }
            rtos_suspend_all();
            auto ret = _internal_link->isLinked();
            rtos_resume_all();
            pros::c::mutex_give(_mutex);
            return ret;
        }

        int32_t send(uint8_t* buffer, int32_t length) {
            if (!_internal_link || !pros::c::mutex_take(_mutex, 2)) {
                return -1;
            }
            rtos_suspend_all();
            auto ret = _internal_link->send(buffer, length);
            rtos_resume_all();
            pros::c::mutex_give(_mutex);
            return ret;
        }

        int32_t send(const char* buffer, int32_t length) {
            return send((uint8_t*) buffer, length);
        }

        int32_t receive(uint8_t* buffer, int32_t length, int32_t timeoutMs = 500) {
            if (!_internal_link || !pros::c::mutex_take(_mutex, 2)) {
                return 0;
            }
            rtos_suspend_all();
            auto ret = _internal_link->receive(buffer, length, timeoutMs);
            rtos_resume_all();
            pros::c::mutex_give(_mutex);
            return ret;
        }

        void received(void (*callback)(uint8_t*, int32_t)) {
            if (!_internal_link || !pros::c::mutex_take(_mutex, 2)) {
                return;
            }
            rtos_suspend_all();
            _internal_link->received(callback);
            pros::c::mutex_give(_mutex);
            rtos_resume_all();
        }
    };
}// namespace lib7405x::device

#endif//INC_7405XV5_SAFE_VEXLINK_H
