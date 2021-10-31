#ifndef INC_7405XV5_TASKS_H
#define INC_7405XV5_TASKS_H

#include "kapi.h"
#include <functional>
#include <string>
#include <unordered_map>

namespace lib7405x {
    class Tasks {
    private:
        Tasks() = default;
        Tasks(Tasks const&) = default;
        Tasks& operator=(Tasks const&) = default;
        static Tasks* _instance;
        inline Tasks* operator->() {
            return _instance;
        }

        std::unordered_map<std::string, void*> _tasks;

    public:
        static Tasks* Instance();
        void createTask(const std::string& id, const std::function<bool(void)>& func, uint32_t prio = TASK_PRIORITY_DEFAULT);
        std::optional<void*> getTask(const std::string& id);
        void suspendTask(const std::string& id);
        void resumeTask(const std::string& id);
        void removeTask(const std::string& id);
        void removeAll();
        void suspendAll();
        void resumeAll();
    };
}// namespace lib7405x
#endif//INC_7405XV5_TASKS_H
