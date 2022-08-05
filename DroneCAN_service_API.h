#ifndef DRONECAN_SERVICE_API_H_
#define DRONECAN_SERVICE_API_H_

#include "DroneCAN_service_base.h"
#include <uavcan.protocol.param.Value.h>
#include "DSDL_to_canard_DTO.h"
#include <list>
using milliseconds = uint32_t;

typedef uavcan_equipment_power_BatteryInfo& (*get_battery_info_t)(void);

typedef struct {
    char data[62];
    uint8_t len;
} parameter_name;
typedef struct {
    parameter_name name;
    uavcan_protocol_param_Value value;
    uavcan_protocol_param_Value default_value;
    uavcan_protocol_param_Value max_value;
    uavcan_protocol_param_Value min_value;
}uavcan_parameter;

class DroneCAN_service : public DroneCAN_service_base {
public:
    explicit DroneCAN_service(uint8_t node_ID = DEFAULT_NODE_ID, droneCAN_handle_error_t handle_error = dummy_function);
    
    uint8_t get_number_of_parameters() {return number_of_parameters;}
    void add_parameter(uavcan_parameter& parameter);
    void remove_parameter(uint8_t parameter_index_from_0) {if (number_of_parameters != 0) --number_of_parameters;}
    uavcan_parameter get_parameter(uint8_t parameter_index_from_0);

    void publish_regularly(get_battery_info_t get_message, uint32_t milliseconds_between_publish);
    
    template<typename T>
    void publish_message(T& message);

    void run_pending_tasks(milliseconds actual_time);

    uint8_t get_node_ID();
    bool is_healthy();

private:
    typedef struct {
        milliseconds last_execution = 0;
        milliseconds time_between_publish = MILLISECONDS_BETWEEN_NODE_STATUS_PUBLISHES;
    }scheduled_time;
    
    scheduled_time message[NUMBER_OF_MESSAGES];
    bool is_time_to_execute_now(type_of_message type, milliseconds actual_time);

    get_battery_info_t _get_battery_info = nullptr;

    uint8_t number_of_parameters = 0;
    std::list<uavcan_parameter> parameter_list{};
};

#endif