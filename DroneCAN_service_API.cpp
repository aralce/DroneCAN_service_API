#include "DroneCAN_service_API.h"
#include <uavcan.protocol.param.GetSet_req.h>
#include <auxiliary_functions.h>
#include <cstring>

void DUMMY_error_handler(DroneCAN_error error) {}

typedef struct{
    CanardInstance *instance;
    CanardRxTransfer *rx_transfer;
}canard_reception_t;
canard_reception_t canard_reception;
bool is_there_canard_message_to_handle = false;
void handle_canard_reception(CanardInstance* ins, CanardRxTransfer* transfer) {
    canard_reception.instance = ins;
    canard_reception.rx_transfer = transfer;
    is_there_canard_message_to_handle = true;
}

bool should_accept_canard_reception(const CanardInstance* ins, uint64_t* out_data_type_signature, uint16_t data_type_id, CanardTransferType transfer_type, uint8_t source_node_id) {
    switch(data_type_id) {
        case UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_ID:
            *out_data_type_signature = UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_SIGNATURE;
            return true;
        default:
            return false;
    }
}

DroneCAN_service::DroneCAN_service(uint8_t node_ID, droneCAN_handle_error_t handle_error) : _node_ID(node_ID) {
    nodeStatus_struct.vendor_specific_status_code = NODE_STATUS_VENDOR_SPECIFIC_STATUS_CODE;

    _handle_error = handle_error == nullptr ? DUMMY_error_handler : handle_error;
    
    message_sender = new DroneCAN_message_sender(canard, can_driver, _handle_error);

    canard.init(handle_canard_reception, should_accept_canard_reception);
    canard.set_node_ID(node_ID);
    
    try_initialize_CAN_bus_driver();
}

void execute_error_handler(droneCAN_handle_error_t handler, DroneCAN_error error_to_send) {
    if (handler != nullptr)
        handler(error_to_send);
}

bool is_can_data_to_read = false;
void onReceive_on_can_bus(int packet_size) {is_can_data_to_read = true;}

void DroneCAN_service::try_initialize_CAN_bus_driver() {
    can_driver.setPins(CAN_BUS_CRX_PIN, CAN_BUS_CTX_PIN);
    _is_healthy = can_driver.begin(CAN_BUS_BAUDRATE);
    if (!_is_healthy)
        _handle_error(DroneCAN_error::ON_INITIALIZATION);
    can_driver.onReceive(onReceive_on_can_bus);
}

bool is_time_to_execute(milliseconds& last_time_executed, milliseconds actual_time, milliseconds time_between_executions);
void handle_incoming_message(DroneCAN_service* droneCAN_service, DroneCAN_message_sender* message_sender);

void DroneCAN_service::run_pending_tasks(milliseconds actual_time) {
    if (is_time_to_execute(last_ms_since_node_status_publish, actual_time, MILLISECONDS_BETWEEN_NODE_STATUS_PUBLISHES)) {
        nodeStatus_struct.uptime_sec = actual_time/1000;
        message_sender->broadcast_message(nodeStatus_struct);
    }
    if (_get_batteryInfo != nullptr && is_time_to_execute(last_ms_since_battery_info_publish, actual_time, ms_between_battery_info_publish)) {
        uavcan_equipment_power_BatteryInfo* battery_info = _get_batteryInfo();
        message_sender->broadcast_message(*battery_info);
    }

    read_can_bus_data_when_is_available(actual_time);
    handle_incoming_message(this, message_sender);
}

bool is_time_to_execute(milliseconds& last_time_executed, milliseconds actual_time, milliseconds time_between_executions) {
    bool is_time_to_execute_now = actual_time - last_time_executed >=  time_between_executions;
    if (is_time_to_execute_now) {
        last_time_executed = actual_time;
        return true;
    }else
        return false;
}

void DroneCAN_service::read_can_bus_data_when_is_available(milliseconds actual_time) {
    if (is_can_data_to_read) {
        CanardCANFrame canard_frame;
        canard_frame.id = can_driver.get_packet_id();
        
        canard_frame.data_len = 8;
        for (int byte = 0; byte < canard_frame.data_len; ++byte)
            canard_frame.data[byte] = can_driver.read();

        uint64_t actual_time_in_microseconds = 1000*(uint64_t)actual_time;
        try_handle_rx_frame_with_canard(canard_frame, actual_time_in_microseconds);
        
        is_can_data_to_read = false;
    }
}

void DroneCAN_service::try_handle_rx_frame_with_canard(CanardCANFrame& frame, uint64_t timestamp_usec) {
    if (canard.handle_rx_frame(frame, timestamp_usec) < 0)
        _handle_error(DroneCAN_error::FAIL_ON_RECEPTION);
}

void handle_incoming_message(DroneCAN_service* droneCAN_service, DroneCAN_message_sender* message_sender) {
    if (is_there_canard_message_to_handle) {
        uavcan_protocol_param_GetSetRequest paramGetSet_request;
        uavcan_protocol_param_GetSetRequest_decode(canard_reception.rx_transfer, &paramGetSet_request);
        
        uavcan_parameter parameter_to_send = droneCAN_service->get_parameter(paramGetSet_request.index);
        if (strcmp(NAME_FOR_INVALID_PARAMETER, (char*)parameter_to_send.name.data) != 0)
            message_sender->send_response_message(parameter_to_send, canard_reception.rx_transfer->source_node_id);

        is_there_canard_message_to_handle = false;
    }
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
        strcpy((char*)invalid_param.name.data, NAME_FOR_INVALID_PARAMETER);
        return invalid_param;
    }
    auto iterator = parameter_list.begin();
    std::advance(iterator, parameter_index_from_0);
    return *iterator;
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, bool value_to_set) {
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, uint16_t value_to_set) {
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
    // if (parameter_index_from_0 >= parameter_list.size())
    //     return false;
    // auto iterator = parameter_list.begin();
    // std::advance(iterator, parameter_index_from_0);
    // iterator->value = package_uavcan_param_value(value_to_set);
    // return true;
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, float value_to_set) {
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
}

template <typename PARAM_VALUE_TYPE>
bool DroneCAN_service::set_generic_parameter_value(uint8_t parameter_index_from_0, PARAM_VALUE_TYPE value_to_set) {
    if (parameter_index_from_0 >= parameter_list.size())
        return false;
    auto iterator = parameter_list.begin();
    std::advance(iterator, parameter_index_from_0);
    iterator->value = package_uavcan_param_value(value_to_set);
    return true;
}