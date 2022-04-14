#pragma once
#include <cstdio>
#include <canard.h>
#include <CAN_bus_interface.h>
#include "DroneCAN_service_configuration.h"
#include <uavcan.equipment.power.BatteryInfo.h>

class DroneCAN_service {
public:
    explicit DroneCAN_service();
    bool is_healthy();
    void publish_message(uavcan_equipment_power_BatteryInfo& battery_info);

private:
    CanardInstance canard_instance;
    uint8_t canard_buffer[LIBCANARD_ALLOCATION_BUFFER_IN_BYTES];
    uint8_t canard_transmission_buffer[UAVCAN_MAX_BYTES_ON_MESSAGE];
    CAN_bus_interface can_interface;
    bool _is_healthy = false;
};
