#include "kapi.h"
#include "system/Tasks.h"
#include "utils/sync.h"


lib7405x::Tasks* lib7405x::Tasks::_instance = nullptr;

lib7405x::Tasks* lib7405x::Tasks::Instance() {
    if (!_instance) {
        _instance = new Tasks;
    }
    return _instance;
}

typedef struct func_payload_s {
    std::function<bool(void)> func;
} func_payload_s_t;

static void task_wrapper(void* func) {
    auto data = (func_payload_s_t*) func;
    while (data->func()) {
        // loop
    }
    delete data;
}

void lib7405x::Tasks::createTask(const std::string& id, const std::function<bool(void)>& func, uint32_t prio) {
    synchronized(this) {
        _tasks[id] = {pros::c::task_create(task_wrapper, new func_payload_s_t{func}, prio, TASK_STACK_DEPTH_DEFAULT, id.c_str())};
    }
}

std::optional<void*> lib7405x::Tasks::getTask(const std::string& id) {
    synchronized(this) {
        if (_tasks.find(id) != _tasks.end()) {
            return _tasks.at(id);
        }
    }
    return std::nullopt;
}

void lib7405x::Tasks::suspendTask(const std::string& id) {
    synchronized(this) {
        if (auto task = _tasks.find(id); task != _tasks.end()) {
            pros::c::task_suspend(task->second);
        }
    }
}

void lib7405x::Tasks::resumeTask(const std::string& id) {
    synchronized(this) {
        if (auto task = _tasks.find(id); task != _tasks.end()) {
            pros::c::task_resume(task->second);
        }
    }
}

void lib7405x::Tasks::removeTask(const std::string& id) {
    synchronized(this) {
        if (auto task = _tasks.find(id); task != _tasks.end()) {
            pros::c::task_delete(task->second);
            _tasks.erase(task);
        }
    }
}

void lib7405x::Tasks::removeAll() {
    synchronized(this) {
        for (auto const& iter : _tasks) {
            pros::c::task_delete(iter.second);
        }
        _tasks.clear();
    }
}

void lib7405x::Tasks::suspendAll() {
    synchronized(this) {
        for (auto const& iter : _tasks) {
            pros::c::task_suspend(iter.second);
        }
    }
}

void lib7405x::Tasks::resumeAll() {
    synchronized(this) {
        for (auto const& iter : _tasks) {
            pros::c::task_resume(iter.second);
        }
    }
}