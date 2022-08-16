#ifndef DRONECAN_SERVICE_API_H_
#define DRONECAN_SERVICE_API_H_

#include <DroneCAN_message_sender.h>
#include <list>

using milliseconds = uint32_t;
using uavcan_parameter = uavcan_protocol_param_GetSetResponse;

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

    //parameters
    uint8_t get_number_of_parameters() {return number_of_parameters;}
    void add_parameter(uavcan_parameter& parameter);
    void remove_parameter(uint8_t parameter_index_from_0);
    uavcan_parameter get_parameter(uint8_t parameter_index_from_0);
    //

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

    uint8_t number_of_parameters = 0;
    std::list<uavcan_parameter> parameter_list{};

    void try_initialize_CAN_bus_driver();
    void try_handle_rx_frame_with_canard(CanardCANFrame& frame, uint64_t timestamp_usec);
};

#endif