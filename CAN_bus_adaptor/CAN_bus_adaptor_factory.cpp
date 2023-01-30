#include "CAN_bus_adaptor_factory.h"
#include "CAN_drivers_implementation/Arduino/Arduino_CAN_driver.h"

CAN_bus_adaptor* CAN_bus_adaptor_factory::get_CAN_bus_adaptor(CAN_bus_driver driver) {
    CAN_bus_adaptor* can = nullptr;
    switch (driver) {
        case CAN_bus_driver::ARDUINO_CAN:
            can = new Arduino_CAN_driver();
            break;
    }
    return can;
}