#pragma once
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <DroneCAN_service_API.h>

class DroneCAN_service_spy 
{
public:
    DroneCAN_service_spy(DroneCAN_service *service_to_spy)
    : droneCAN_service(service_to_spy) {}

    CAN_bus_adaptor* spy_bus_adaptor() { return &droneCAN_service->can_driver;}
    droneCAN_handle_error_t spy_handle_error() { return droneCAN_service->_handle_error;}

private:
    DroneCAN_service *droneCAN_service = nullptr;
};

class CAN_bus_custom : public CAN_bus_adaptor
{

};