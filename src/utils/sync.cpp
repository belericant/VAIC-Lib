#include "utils/sync.h"

std::unordered_map<uintptr_t, pros::mutex_t> Guard::_mutexes{};

__attribute__((constructor(103))) void arr_lock_init() {
    Guard::_arrayLock = pros::c::mutex_create();
}