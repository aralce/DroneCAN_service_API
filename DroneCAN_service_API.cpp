#include "DroneCAN_service_API.h"
#include "DroneCAN_service_configuration.h"
#include <cstring>

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

void DroneCAN_service::remove_parameter(uint8_t parameter_index_from_0) {
    if (number_of_parameters != 0) {
        --number_of_parameters;
        auto iterator = parameter_list.begin();
        std::advance(iterator, parameter_index_from_0);
        parameter_list.erase(iterator);
    }
}

uavcan_parameter DroneCAN_service::get_parameter(uint8_t parameter_index_from_0) {
    if (parameter_list.empty() || parameter_index_from_0 >= parameter_list.size()) {
        uavcan_parameter invalid_param;
        strcpy((char*)invalid_param.name.data, "INVALID");
        return invalid_param;
    }
    auto iterator = parameter_list.begin();
    std::advance(iterator, parameter_index_from_0);
    return *iterator;
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

#define count_of(x) sizeof(x)/sizeof(x[0])
void DroneCAN_service::respond_with_parameter_data(uint8_t parameter_index_from_0) {
    uavcan_parameter param_to_send = get_parameter(parameter_index_from_0);
    uint8_t buffer[UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_MAX_SIZE];
    uavcan_protocol_param_GetSetResponse_encode(&param_to_send, buffer);
    
    canard_message_type_info_t type_info = {.signature = UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_SIGNATURE,
                                            .id = UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_ID,
                                            .priority = CANARD_TRANSFER_PRIORITY_MEDIUM};
    canard_message_data_t message_data = {.ptr = (void*)buffer,
                                          .length = count_of(buffer)};
    canard.send_response(type_info, message_data);
    canard.is_txQueue_empty();
    canard.peekTxQueue();
    canard.popTxQueue();
    canard.is_txQueue_empty();
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