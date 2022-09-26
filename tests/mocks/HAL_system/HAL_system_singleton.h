#pragma once
#include <cstdint>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

class HAL_system_api {
public:

    void set_millisecs_since_init(uint32_t ms) {
        ms_since_init = ms;
    }

    uint32_t millisecs_since_init() {
        return ms_since_init;
    }

    uint64_t usecs_since_init() {
        mock().actualCall("HAL_system_api->usecs_since_init");
        return (uint64_t)mock().returnUnsignedLongLongIntValueOrDefault(0);
    }

    void restart() {
        mock().actualCall("HAL_system_api->restart");
    }

    void print_to_USB(char* message, uint32_t baudrate) {
        mock().actualCall("HAL_system_api->print_to_USB")
              .withStringParameter("message", message)
              .withUnsignedIntParameter("baudrate", baudrate);
    }
private:
    uint32_t ms_since_init = 0;

};

class HAL_system_singleton {
public:
    static HAL_system_api* get_HAL_system_instance();
    static void delete_instance();

private:
    static HAL_system_api* hal_system_instance;
};