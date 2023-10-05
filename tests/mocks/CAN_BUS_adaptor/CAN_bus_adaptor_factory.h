#pragma once
#include "CAN_bus_adaptor.h"

class CAN_bus_adapter_singleton {
public:
    static CAN_bus_adaptor* get_CAN_bus_adaptor()
    {
        if (can_bus == nullptr)
            can_bus = new CAN_bus_adaptor();
        return can_bus;
    }

    static void clear_instances()
    {
        if (can_bus != nullptr)
        {
            delete can_bus;
            can_bus = nullptr;
        }
    }

private:
    static CAN_bus_adaptor* can_bus;
};