#pragma once
#include <DroneCAN_service_base.h>

class DroneCAN_service : public DroneCAN_service_base {
public:
    explicit DroneCAN_service(droneCAN_handle_error_t handle_error = dummy_function);
    explicit DroneCAN_service(uint8_t node_ID, droneCAN_handle_error_t handle_error = dummy_function);
    
    void publish_regularly(uavcan_equipment_power_BatteryInfo& battery_info, uint32_t milliseconds_between_publish);
    void publish_message(uavcan_equipment_power_BatteryInfo& battery_info);

    void run_pending_tasks(uint32_t actual_time_in_milliseconds) {
        if(actual_time_in_milliseconds - _last_time >= _time_between_publishes) {
            _last_time = actual_time_in_milliseconds;
            publish_message(_message_to_publish);
        }
    }

    uint8_t get_node_ID();
    bool is_healthy();

private:
    uint32_t _time_between_publishes;
    uint32_t _last_time = 0;
    uavcan_equipment_power_BatteryInfo _message_to_publish;
};
