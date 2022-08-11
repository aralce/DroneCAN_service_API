#ifndef DRONECAN_SERVICE_API_H_
#define DRONECAN_SERVICE_API_H_

#include "DroneCAN_message_sender.h"

// using milliseconds = uint32_t;

// typedef uavcan_equipment_power_BatteryInfo& (*get_battery_info_t)(void);

// using uavcan_parameter = uavcan_protocol_param_GetSetResponse;

class DroneCAN_service {
public:
    explicit DroneCAN_service(uint8_t node_ID = DEFAULT_NODE_ID, droneCAN_handle_error_t handle_error = nullptr);

    bool is_healthy() {return _is_healthy;}
    uint8_t get_node_ID() {return _node_ID;}
protected:
    droneCAN_handle_error_t _handle_error = nullptr;

private:
    Canard canard{LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, UAVCAN_MAX_BYTES_ON_MESSAGE};
    CAN_bus_adaptor can_driver;
    
    bool _is_healthy = true;
    uint8_t _node_ID;

    void try_initialize_CAN_bus_driver();
};


// class DroneCAN_service {
// public:
//     explicit DroneCAN_service(uint8_t node_ID = DEFAULT_NODE_ID, droneCAN_handle_error_t handle_error = nullptr);
    
//     //GROUP param_get_set
//     uint8_t get_number_of_parameters() {return number_of_parameters;}
//     void add_parameter(uavcan_parameter& parameter);
//     void remove_parameter(uint8_t parameter_index_from_0);
//     uavcan_parameter get_parameter(uint8_t parameter_index_from_0);
//     //

//     void publish_regularly(get_battery_info_t get_message, uint32_t milliseconds_between_publish);
    
//     template<typename T>
//     void publish_message(T& message);

//     void run_pending_tasks(milliseconds actual_time);

//     uint8_t get_node_ID();
//     bool is_healthy();

// protected:
//     void respond_with_parameter_data(uint8_t parameter_index_from_0);

// private:
//     DroneCAN_message_sender message_sender(DEFAULT_NODE_ID);
//     typedef struct {
//         milliseconds last_execution = 0;
//         milliseconds time_between_publish = MILLISECONDS_BETWEEN_NODE_STATUS_PUBLISHES;
//     }scheduled_time;
    
//     scheduled_time message[NUMBER_OF_MESSAGES];
//     bool is_time_to_execute_now(type_of_message type, milliseconds actual_time);

//     get_battery_info_t _get_battery_info = nullptr;

//     uint8_t number_of_parameters = 0;
//     std::list<uavcan_parameter> parameter_list{};
// };

#endif