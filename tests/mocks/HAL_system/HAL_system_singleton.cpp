#include "HAL_system_singleton.h"

HAL_system_api* HAL_system_singleton::hal_system_instance = nullptr;

HAL_system_api* HAL_system_singleton::get_HAL_system_instance() {
    if (hal_system_instance == nullptr)
        hal_system_instance = new HAL_system_api;
    return hal_system_instance;
}

void HAL_system_singleton::delete_instance() {
    if (hal_system_instance != nullptr) {
        delete hal_system_instance;
        hal_system_instance = nullptr;
    }
}