#pragma once
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <DroneCAN_service_API.h>

class DroneCAN_service_spy 
{
public:
    DroneCAN_service_spy(DroneCAN_service *service_to_spy)
    : droneCAN_service(service_to_spy) {}

    CAN_bus_adaptor* spy_bus_adaptor() { return droneCAN_service->can_driver;}
    droneCAN_handle_error_t spy_handle_error() { return droneCAN_service->_handle_error;}

private:
    DroneCAN_service *droneCAN_service = nullptr;
};

class CAN_bus_custom : public CAN_bus_adaptor
{
    bool begin(CAN_bitrate baudrate) override
    {
        mock().actualCall("CAN_bus_custom->begin")
              .withLongIntParameter("baudrate", (long int)baudrate);
        return mock().returnBoolValueOrDefault(true);
    }

    void setPins(int rx, int tx) override
    {
        mock().actualCall("CAN_bus_custom->setPins")
             .withIntParameter("rx", rx)
             .withIntParameter("tx", tx);
    }

    bool add_master_mailbox() override
    {
        mock().actualCall("CAN_bus_custom->add_master_mailbox");
        return mock().returnBoolValueOrDefault(true);
    }
};