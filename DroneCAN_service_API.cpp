#include "DroneCAN_service_API.h"
#include <cstring>

void DUMMY_error_handler(DroneCAN_error error) {}

DroneCAN_service::DroneCAN_service(uint8_t node_ID, droneCAN_handle_error_t handle_error) : _node_ID(node_ID) {
    nodeStatus_struct.vendor_specific_status_code = NODE_STATUS_VENDOR_SPECIFIC_STATUS_CODE;

    _handle_error = handle_error == nullptr ? DUMMY_error_handler : handle_error;
    
    message_sender = new DroneCAN_message_sender(canard, can_driver, _handle_error);

    canard.init();
    canard.set_node_ID(node_ID);
    
    try_initialize_CAN_bus_driver();
}

void execute_error_handler(droneCAN_handle_error_t handler, DroneCAN_error error_to_send) {
    if (handler != nullptr)
        handler(error_to_send);
}

void DroneCAN_service::try_initialize_CAN_bus_driver() {
    can_driver.setPins(CAN_BUS_CRX_PIN, CAN_BUS_CTX_PIN);
    _is_healthy = can_driver.begin(CAN_BUS_BAUDRATE);
    if (!_is_healthy)
        _handle_error(DroneCAN_error::ON_INITIALIZATION);
}

bool is_time_to_execute(milliseconds& last_time_executed, milliseconds actual_time, milliseconds time_between_executions);

void DroneCAN_service::run_pending_tasks(milliseconds actual_time) {
    if (is_time_to_execute(last_ms_since_node_status_publish, actual_time, MILLISECONDS_BETWEEN_NODE_STATUS_PUBLISHES)) {
        nodeStatus_struct.uptime_sec = actual_time/1000;
        message_sender->broadcast_message(nodeStatus_struct);
    }
    if (_get_batteryInfo != nullptr && is_time_to_execute(last_ms_since_battery_info_publish, actual_time, ms_between_battery_info_publish)) {
        uavcan_equipment_power_BatteryInfo* battery_info = _get_batteryInfo();
        message_sender->broadcast_message(*battery_info);
    }
}

bool is_time_to_execute(milliseconds& last_time_executed, milliseconds actual_time, milliseconds time_between_executions) {
    bool is_time_to_execute_now = actual_time - last_time_executed >=  time_between_executions;
    if (is_time_to_execute_now) {
        last_time_executed = actual_time;
        return true;
    }else
        return false;
}

// void DroneCAN_service::add_parameter(uavcan_parameter& parameter) {
//     if (number_of_parameters < MAX_NUMBER_OF_PARAMETERS) {
//         ++number_of_parameters;
//         parameter_list.push_back(parameter);
//     }
// }

// void DroneCAN_service::remove_parameter(uint8_t parameter_index_from_0) {
//     if (number_of_parameters != 0) {
//         --number_of_parameters;
//         auto iterator = parameter_list.begin();
//         std::advance(iterator, parameter_index_from_0);
//         parameter_list.erase(iterator);
//     }
// }

// uavcan_parameter DroneCAN_service::get_parameter(uint8_t parameter_index_from_0) {
//     if (parameter_list.empty() || parameter_index_from_0 >= parameter_list.size()) {
//         uavcan_parameter invalid_param;
//         strcpy((char*)invalid_param.name.data, "INVALID");
//         return invalid_param;
//     }
//     auto iterator = parameter_list.begin();
//     std::advance(iterator, parameter_index_from_0);
//     return *iterator;
// }