#ifndef DRONECAN_SERVICE_API_H_
#define DRONECAN_SERVICE_API_H_

#include <list>

#ifdef IS_RUNNING_TESTS
    #include "tests/mocks/DroneCAN_message_sender/DroneCAN_message_sender.h"
    #include "tests/mocks/CAN_BUS_adaptor/CAN_bus_adaptor.h"
#else
    #include "implementation/DroneCAN_message_sender.h"
    #include "Interface_to_implement/CAN_driver_interface.h"
#endif

using microseconds = uint64_t;
using milliseconds = uint32_t;

typedef struct
{
    struct uavcan_protocol_param_Value value;
    struct uavcan_protocol_param_Value default_value;
    struct uavcan_protocol_param_NumericValue max_value;
    struct uavcan_protocol_param_NumericValue min_value;
    struct { uint8_t len; uint8_t data[92]; }name;
    bool is_read_only;
}uavcan_parameter_t;

// using uavcan_parameter_t = uavcan_protocol_param_GetSetResponse;

enum class uavcan_message_type{BATTERY_INFO};

typedef struct
{
    struct uavcan_protocol_SoftwareVersion software_version;
    struct uavcan_protocol_HardwareVersion hardware_version;
    struct { uint8_t len; uint8_t data[80]; }name;
}node_info_data_t;

class DroneCAN_service {
public:
    explicit DroneCAN_service(CAN_driver_interface& can_bus,
                              uint8_t node_ID = DEFAULT_NODE_ID,
                              droneCAN_handle_error_t handle_error = nullptr);

    ~DroneCAN_service() {delete message_sender;};

    //returns the milliseconds required until the next call.
    uint32_t run_pending_tasks(microseconds actual_time);

    bool is_CAN_bus_inactive(milliseconds to_consider_can_bus_inactive, milliseconds actual_time);

    template <typename UAVCAN_MESSAGE>
    void publish_message(UAVCAN_MESSAGE& uavcan_message)
    {
        message_sender->broadcast_message(uavcan_message);
    }

    void register_for_regular_publish(uavcan_equipment_power_BatteryInfo* batteryInfo_msg,
                                      microseconds time_between_publish)
    {
        _batteryInfo_msg = batteryInfo_msg;
        microsecs_between_battery_info_publish = time_between_publish;
    }

    //parameters
    uint8_t get_number_of_parameters() {return number_of_parameters;}
    
    void add_parameter(uavcan_parameter_t& parameter);
    void remove_parameter(uint8_t parameter_index_from_0);
    
    uavcan_parameter_t get_parameter_by_name(const char* name);
    uavcan_parameter_t get_parameter(uint8_t parameter_index_from_0);
    
    bool set_parameter_value_by_name(const char* name, void* pointer_to_value_to_set, uavcan_protocol_param_Value_type_t data_type);
    bool set_parameter_value_by_name(const char* name, bool value_to_set);
    bool set_parameter_value_by_name(const char* name, int32_t value_to_set);
    bool set_parameter_value_by_name(const char* name, int64_t value_to_set);
    bool set_parameter_value_by_name(const char* name, float value_to_set);
    bool set_parameter_value_by_name(const char* name, char* value_to_set);
    bool set_parameter_value(uint8_t parameter_index_from_0, bool value_to_set);
    bool set_parameter_value(uint8_t parameter_index_from_0, int32_t value_to_set);
    bool set_parameter_value(uint8_t parameter_index_from_0, int64_t value_to_set);
    bool set_parameter_value(uint8_t parameter_index_from_0, float value_to_set);
    bool set_parameter_value(uint8_t parameter_index_from_0, char* value_to_set);
    //

    bool is_healthy() {return _is_healthy;}
    uint8_t get_node_ID() {return _node_ID;}

    void set_node_info(const node_info_data_t& node_info_data);

private:
    friend class DroneCAN_service_spy;
    Canard canard;
    CAN_driver_interface& can_driver;
    DroneCAN_message_sender* message_sender;

    droneCAN_handle_error_t _handle_error = nullptr;

    bool _is_healthy = true;
    uint8_t _node_ID;
    uint32_t node_IDs_on_bus[4]{};

    milliseconds ms_on_last_rx = 0;
    microseconds last_microsecs_since_node_status_publish = 0;
    uavcan_protocol_NodeStatus nodeStatus_struct{};
    uavcan_protocol_GetNodeInfoResponse nodeInfo{};

    uavcan_equipment_power_BatteryInfo*  _batteryInfo_msg = nullptr;
    microseconds microsecs_between_battery_info_publish = 0;
    microseconds last_microsecs_since_battery_info_publish = 0;

    microseconds last_microsecs_since_clear_node_ID_reg = 0;

    uint8_t number_of_parameters = 0;
    std::list<uavcan_parameter_t> parameter_list{};

    void initialize_system(uint8_t node_ID, droneCAN_handle_error_t handle_error);

    bool is_time_to_publish_nodeStatus_msg(microseconds actual_time);
    uint32_t get_ms_until_next_nodeStatus_publish(microseconds actual_time);
    bool is_time_to_publish_batteryInfo_msg(microseconds actual_time);
    uint32_t get_ms_until_next_batteryInfo_publish(microseconds actual_time);

    bool is_time_to_clear_bus_nodes_ID_register(microseconds actual_time);

    void read_can_bus_data_when_is_available(microseconds actual_time);
    void handle_incoming_message(Canard& canard, DroneCAN_message_sender* message_sender);
    void process_nodeStatus_reception(uint8_t source_node_id);

    template <typename PARAM_VALUE_TYPE>
    bool set_generic_parameter_value_by_name(const char* name, PARAM_VALUE_TYPE value_to_set);
    
    template <typename PARAM_VALUE_TYPE>
    bool set_generic_parameter_value(uint8_t parameter_index_from_0, PARAM_VALUE_TYPE value_to_set);
};

#endif