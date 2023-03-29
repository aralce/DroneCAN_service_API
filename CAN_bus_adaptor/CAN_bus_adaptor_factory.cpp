#include "CAN_bus_adaptor_factory.h"
#ifndef IS_RUNNING_TESTS
    #include "CAN_drivers_implementation/Arduino/Arduino_CAN_driver.h"
    #include "CAN_drivers_implementation/ESP_IDF/ESP_IDF_CAN_driver.h"
#endif

CAN_bus_adaptor* CAN_bus_adaptor_factory::get_CAN_bus_adaptor() {
    CAN_bus_adaptor* can = nullptr;
    #if defined(ESP32_WITH_ARDUINO)
        can = new Arduino_CAN_driver();
    #elif defined(ESP32_WITH_ESP_IDF)
        can = new ESP_IDF_CAN_driver();
    #else
        #error "CAN_bus_adaptor without driver is not supported"
    #endif
    return can;
}