#ifndef DRONECAN_SERVICE_API_H_
#define DRONECAN_SERVICE_API_H_

#include <DroneCAN_message_sender.h>
#include <list>

using microseconds = uint64_t;
using uavcan_parameter = uavcan_protocol_param_GetSetResponse;

enum class uavcan_message_type{BATTERY_INFO};

class DroneCAN_service {
public:
    explicit DroneCAN_service(uint8_t node_ID = DEFAULT_NODE_ID, droneCAN_handle_error_t handle_error = nullptr);
    ~DroneCAN_service() {delete message_sender;};

    void run_pending_tasks(microseconds actual_time);

    template <typename UAVCAN_MESSAGE>
    void publish_message(UAVCAN_MESSAGE& uavcan_message) {
        message_sender->broadcast_message(uavcan_message);
    }

    void register_for_regular_publish(uavcan_equipment_power_BatteryInfo* (*get_battery_info)(), microseconds time_between_publish) {
        _get_batteryInfo = get_battery_info;
        ms_between_battery_info_publish = time_between_publish;
    }

    //parameters
    uint8_t get_number_of_parameters() {return number_of_parameters;}
    
    void add_parameter(uavcan_parameter& parameter);
    void remove_parameter(uint8_t parameter_index_from_0);
    
    uavcan_parameter get_parameter_by_name(const char* name);
    uavcan_parameter get_parameter(uint8_t parameter_index_from_0);
    
    bool set_parameter_value_by_name(const char* name, void* pointer_to_value_to_set, uavcan_protocol_param_Value_type_t data_type);
    bool set_parameter_value_by_name(const char* name, bool value_to_set);
    bool set_parameter_value_by_name(const char* name, int32_t value_to_set) {return set_parameter_value_by_name(name, (int64_t)value_to_set);}
    bool set_parameter_value_by_name(const char* name, int64_t value_to_set);
    bool set_parameter_value_by_name(const char* name, float value_to_set);
    bool set_parameter_value(uint8_t parameter_index_from_0, bool value_to_set);
    bool set_parameter_value(uint8_t parameter_index_from_0, int32_t value_to_set);
    bool set_parameter_value(uint8_t parameter_index_from_0, float value_to_set);
    //

    bool is_healthy() {return _is_healthy;}
    uint8_t get_node_ID() {return _node_ID;}

protected:
    droneCAN_handle_error_t _handle_error = nullptr;
    uavcan_protocol_NodeStatus nodeStatus_struct{};

private:
    Canard canard{LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, UAVCAN_MAX_BYTES_ON_MESSAGE};
    CAN_bus_adaptor can_driver;
    DroneCAN_message_sender* message_sender;

    bool _is_healthy = true;
    uint8_t _node_ID;
    microseconds last_ms_since_node_status_publish = 0;

    typedef uavcan_equipment_power_BatteryInfo* (*get_batteryInfo_handler)();
    get_batteryInfo_handler  _get_batteryInfo= nullptr;
    microseconds ms_between_battery_info_publish = 0;
    microseconds last_ms_since_battery_info_publish = 0;
    microseconds last_ms_since_clean_of_canard = 0;

    uint8_t number_of_parameters = 0;
    std::list<uavcan_parameter> parameter_list{};

    void try_initialize_CAN_bus_driver();
    void read_can_bus_data_when_is_available(microseconds actual_time);
    void try_handle_rx_frame_with_canard(CanardCANFrame& frame, uint64_t timestamp_usec);

    template <typename PARAM_VALUE_TYPE>
    bool set_generic_parameter_value_by_name(const char* name, PARAM_VALUE_TYPE value_to_set);
    
    template <typename PARAM_VALUE_TYPE>
    bool set_generic_parameter_value(uint8_t parameter_index_from_0, PARAM_VALUE_TYPE value_to_set);
};

#endif