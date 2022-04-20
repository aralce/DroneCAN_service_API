#pragma once
#include <DroneCAN_service_base.h>

class DroneCAN_service : public DroneCAN_service_base {
public:
    explicit DroneCAN_service(droneCAN_handle_error_t handle_error = dummy_function);
    explicit DroneCAN_service(uint8_t node_ID, droneCAN_handle_error_t handle_error = dummy_function);
    void publish_message(uavcan_equipment_power_BatteryInfo& battery_info);
    uint8_t get_node_ID();
    bool is_healthy();
};
