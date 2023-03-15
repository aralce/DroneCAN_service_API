#include "CAN_bus_adaptor_factory.h"
#ifndef IS_RUNNING_TESTS
    #include "CAN_drivers_implementation/Arduino/Arduino_CAN_driver.h"
    #include "CAN_drivers_implementation/ESP_IDF/ESP_IDF_CAN_driver.h"
#endif

CAN_bus_adaptor* CAN_bus_adaptor_factory::get_CAN_bus_adaptor(CAN_bus_driver driver) {
    CAN_bus_adaptor* can = nullptr;
    switch (driver) {
        case CAN_bus_driver::ARDUINO_CAN:
            can = new Arduino_CAN_driver();
            break;
        case CAN_bus_driver::ESP_IDF:
            can = new ESP_IDF_CAN_driver();
            break;
    }
    return can;
}