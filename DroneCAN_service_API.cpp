#include "DroneCAN_service_API.h"
#include "DroneCAN_service_configuration.h"

DroneCAN_service::DroneCAN_service(uint8_t node_ID, droneCAN_handle_error_t handle_error)
    : DroneCAN_service_base(node_ID, handle_error) 
{
    canard.init();
    canard.set_node_ID(node_ID);
    try_initialize_CAN_bus_driver();
}

void DroneCAN_service::add_parameter(uavcan_parameter& parameter) {
    if (number_of_parameters < MAX_NUMBER_OF_PARAMETERS) {
        ++number_of_parameters;
        parameter_list.push_back(parameter);
    }
}

uavcan_parameter DroneCAN_service::get_parameter(uint8_t parameter_index_from_0) {
    auto iter = parameter_list.begin();
    std::advance(iter, parameter_index_from_0);
    return *iter;
}

void DroneCAN_service::publish_regularly(get_battery_info_t get_message, milliseconds time_between_publish) {
    if (time_between_publish == 0) {
        _get_battery_info = nullptr;
        return;
    }
    _get_battery_info = get_message;
    publish_message(_get_battery_info());
    message[BATTERY_INFO].time_between_publish = time_between_publish;
}

template<typename T>
void DroneCAN_service::publish_message(T& message) {
    DSDL_to_canard_DTO data_transfer_object(message);
    publish_generic_message(data_transfer_object.get_type_info(), data_transfer_object.get_data());
}

void DroneCAN_service::run_pending_tasks(milliseconds actual_time) {
    if (is_time_to_execute_now(NODE_STATUS, actual_time)) {
        message[NODE_STATUS].last_execution = actual_time;
        uavcan_protocol_NodeStatus node_status{};
        node_status.uptime_sec = actual_time/1000;
        node_status.vendor_specific_status_code = NODE_STATUS_VENDOR_SPECIFIC_STATUS_CODE;
        publish_message(node_status);
    }
    if (is_time_to_execute_now(BATTERY_INFO, actual_time)) {
        message[BATTERY_INFO].last_execution = actual_time;
        if (_get_battery_info != nullptr)
            publish_message(_get_battery_info());
    }

}

bool DroneCAN_service::is_time_to_execute_now(type_of_message type, milliseconds actual_time) {
    return actual_time - message[type].last_execution >= message[type].time_between_publish;
}

uint8_t DroneCAN_service::get_node_ID() {
    return _node_ID;
}

bool DroneCAN_service::is_healthy() {
    return _is_healthy;
}