#ifndef DRONECAN_SERVICE_API_H_
#define DRONECAN_SERVICE_API_H_

#include <DroneCAN_message_sender.h>

using milliseconds = uint32_t;
// using uavcan_parameter = uavcan_protocol_param_GetSetResponse;

enum class uavcan_message_type{BATTERY_INFO};

class DroneCAN_service {
public:
    explicit DroneCAN_service(uint8_t node_ID = DEFAULT_NODE_ID, droneCAN_handle_error_t handle_error = nullptr);
    ~DroneCAN_service() {delete message_sender;};

    void run_pending_tasks(milliseconds actual_time);

    template <typename UAVCAN_MESSAGE>
    void publish_message(UAVCAN_MESSAGE& uavcan_message) {
        message_sender->broadcast_message(uavcan_message);
    }

    void register_for_regular_publish(uavcan_equipment_power_BatteryInfo* (*get_battery_info)(), milliseconds time_between_publish) {
        _get_batteryInfo = get_battery_info;
        ms_between_battery_info_publish = time_between_publish;
    }

    bool is_healthy() {return _is_healthy;}
    uint8_t get_node_ID() {return _node_ID;}

protected:
    droneCAN_handle_error_t _handle_error = nullptr;
    uavcan_protocol_NodeStatus nodeStatus_struct{};

private:
    typedef void* (*function_pointer);
    function_pointer callback_functions[NUMBER_OF_MESSAGES]{};
    
    Canard canard{LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, UAVCAN_MAX_BYTES_ON_MESSAGE};
    CAN_bus_adaptor can_driver;
    DroneCAN_message_sender* message_sender;

    bool _is_healthy = true;
    uint8_t _node_ID;
    milliseconds last_ms_since_node_status_publish = 0;

    typedef uavcan_equipment_power_BatteryInfo* (*get_batteryInfo_handler)();
    get_batteryInfo_handler  _get_batteryInfo= nullptr;
    milliseconds ms_between_battery_info_publish = 0;
    milliseconds last_ms_since_battery_info_publish = 0;

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