#pragma once
#include "DroneCAN_service_base.h"
// #include "DSDL_to_canard_DTO.h"
using milliseconds = uint32_t;

// typedef uavcan_equipment_power_BatteryInfo& (*get_battery_info_t)(void);

class DroneCAN_service : public DroneCAN_service_base {
public:
    explicit DroneCAN_service(uint8_t node_ID = DEFAULT_NODE_ID, droneCAN_handle_error_t handle_error = dummy_function);
    
//     void publish_regularly(get_battery_info_t get_message, uint32_t milliseconds_between_publish);
    
//     template<typename T>
//     void publish_message(T& message) {
//         DSDL_to_canard_DTO data_transfer_object(message);
//         publish_generic_message(data_transfer_object.get_type_info(), data_transfer_object.get_data());
//     }

//     void run_pending_tasks(milliseconds actual_time);

//     uint8_t get_node_ID();
//     bool is_healthy();

// private:
//     typedef struct {
//         milliseconds last_execution = 0;
//         milliseconds time_between_publish = MILLISECONDS_BETWEEN_NODE_STATUS_PUBLISHES;
//     }scheduled_time;
    
//     scheduled_time message[NUMBER_OF_MESSAGES];
//     bool is_time_to_execute_now(type_of_message type, milliseconds actual_time);

//     get_battery_info_t _get_battery_info = nullptr;
};
