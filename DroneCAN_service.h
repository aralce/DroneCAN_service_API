#pragma once
#include <cstdio>
#include <Canard_wrapper.h>
#include <CAN_bus_adaptor.h>
#include "DroneCAN_service_configuration.h"
#include <uavcan.equipment.power.BatteryInfo.h>

class DroneCAN_service {
public:
    explicit DroneCAN_service();
    bool is_healthy();
    void publish_message(uavcan_equipment_power_BatteryInfo& battery_info);

protected:
    Canard<LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, UAVCAN_MAX_BYTES_ON_MESSAGE> canard;
    CAN_bus_adaptor can_driver;
    bool _is_healthy = false;
};
