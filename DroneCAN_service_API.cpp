#include "DroneCAN_service_API.h"
#include <uavcan.protocol.param.GetSet_req.h>
#include <uavcan.protocol.GetNodeInfo.h>
#include <auxiliary_functions.h>
#include <cstring>
#include <Arduino.h>

#ifdef IS_RUNNING_TESTS
    #include <mocks/HAL_system/HAL_system_singleton.h>
#else
    #include <HAL_system/HAL_system_singleton.h>
#endif

CanardPoolAllocatorStatistics statistics{};

void DUMMY_error_handler(DroneCAN_error error) {}

typedef struct{
    CanardInstance *instance;
    CanardRxTransfer *rx_transfer;
    uint8_t source_node_id;
}canard_reception_t;

canard_reception_t canard_reception;
bool is_there_canard_message_to_handle = false;

void handle_canard_reception(CanardInstance* ins, CanardRxTransfer* transfer) {
    canard_reception.instance = ins;
    canard_reception.rx_transfer = transfer;
    canard_reception.source_node_id = transfer->source_node_id;
    is_there_canard_message_to_handle = true;
}

bool should_accept_canard_reception(const CanardInstance* ins, uint64_t* out_data_type_signature, uint16_t data_type_id, CanardTransferType transfer_type, uint8_t source_node_id) {

    switch(data_type_id) {
        case UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_ID:
            *out_data_type_signature = UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_SIGNATURE;
            return true;
        case UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_ID:
            *out_data_type_signature = UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_SIGNATURE;
            return true;
        default:
            return false;
    }
}

DroneCAN_service::DroneCAN_service(uint8_t node_ID, droneCAN_handle_error_t handle_error) : _node_ID(node_ID) {
    _handle_error = handle_error == nullptr ? DUMMY_error_handler : handle_error;
    
    message_sender = new DroneCAN_message_sender(canard, *can_driver, _handle_error);

    canard.init(handle_canard_reception, should_accept_canard_reception);
    canard.set_node_ID(node_ID);
    
    try_initialize_CAN_bus_driver();
}

bool is_can_data_to_read = false;
uint32_t ms_since_last_rx = 0;
void onReceive_on_can_bus(int packet_size) {
    is_can_data_to_read = true;
    // Serial.println("CAN BUS MESSAGE RECIEVED");
    ms_since_last_rx = HAL_system_singleton::get_HAL_system_instance()->millisecs_since_init();
}

void DroneCAN_service::try_initialize_CAN_bus_driver() {
    can_driver->setPins(CAN_BUS_CRX_PIN, CAN_BUS_CTX_PIN);
    _is_healthy = can_driver->begin(CAN_BUS_BAUDRATE);
    if (!_is_healthy)
        _handle_error(DroneCAN_error::ON_INITIALIZATION);
    can_driver->onReceive(onReceive_on_can_bus);
}

bool DroneCAN_service::is_CAN_bus_inactive(uint32_t ms_to_consider_can_bus_inactive) {
    uint32_t ms_since_init = HAL_system_singleton::get_HAL_system_instance()->millisecs_since_init();
    if (ms_since_init - ms_since_last_rx >= ms_to_consider_can_bus_inactive)
        return true;
    return false;
}

bool is_time_to_execute(microseconds& last_time_executed, microseconds actual_time, microseconds time_between_executions);

#define MICROSECONDS_IN_SECOND (uint32_t)1e6
void DroneCAN_service::run_pending_tasks(microseconds actual_time) {
    if (is_time_to_execute(last_microsecs_since_node_status_publish, actual_time, MICROSECONDS_BETWEEN_NODE_STATUS_PUBLISHES)) {
        nodeStatus_struct.uptime_sec = actual_time/MICROSECONDS_IN_SECOND;
        message_sender->broadcast_message(nodeStatus_struct);
    }
    
    if (_get_batteryInfo != nullptr && is_time_to_execute(last_microsecs_since_battery_info_publish, actual_time, microsecs_between_battery_info_publish)) {
        uavcan_equipment_power_BatteryInfo* battery_info = _get_batteryInfo();
        message_sender->broadcast_message(*battery_info);
    }

    read_can_bus_data_when_is_available(actual_time);
    handle_incoming_message(canard, message_sender);
}

bool is_time_to_execute(microseconds& last_time_executed, microseconds actual_time, microseconds time_between_executions) {
    bool is_time_to_execute_now = actual_time - last_time_executed >=  time_between_executions;
    if (is_time_to_execute_now) {
        last_time_executed = actual_time;
        return true;
    }else
        return false;
}

void DroneCAN_service::read_can_bus_data_when_is_available(microseconds actual_time) {
    if (is_can_data_to_read) {
        CanardCANFrame canard_frame;
        canard_frame.id = can_driver->get_packet_id();
        
        canard_frame.data_len = 8;
        for (int byte = 0; byte < canard_frame.data_len; ++byte)
            canard_frame.data[byte] = can_driver->read();

        canard.handle_rx_frame(canard_frame, actual_time);
        
        is_can_data_to_read = false;
    }
}

// void DroneCAN_service::try_handle_rx_frame_with_canard(CanardCANFrame& frame, uint64_t timestamp_usec) { //TODO: handle error only on desired error codes
//     if (canard.handle_rx_frame(frame, timestamp_usec) < 0)
//         _handle_error(DroneCAN_error::FAIL_ON_RECEPTION);
// }

void DroneCAN_service::handle_incoming_message(Canard& canard, DroneCAN_message_sender* message_sender) {
    if (is_there_canard_message_to_handle) {
        uavcan_protocol_GetNodeInfoResponse get_node_info_response{};
        uavcan_protocol_param_GetSetRequest paramGetSet_request{};

        switch(canard_reception.rx_transfer->data_type_id) {
            case UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_ID:
                get_node_info_response.status = nodeStatus_struct;
                strcpy((char*)get_node_info_response.name.data, DRONECAN_NODE_NAME);
                get_node_info_response.name.len = strlen(DRONECAN_NODE_NAME);
                message_sender->send_response_message(get_node_info_response, canard_reception.source_node_id);
                break;

            case UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_ID:
                uavcan_protocol_param_GetSetRequest_decode(canard_reception.rx_transfer, &paramGetSet_request);

                static uavcan_parameter parameter_to_send; //this parameter must be on scope
                parameter_to_send = this->get_parameter(paramGetSet_request.index);

                if (strcmp(NAME_FOR_INVALID_PARAMETER, (char*)parameter_to_send.name.data) != 0)
                    message_sender->send_response_message(parameter_to_send, canard_reception.source_node_id);
                break;
        }
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

bool DroneCAN_service::set_parameter_value_by_name(const char* name, void* pointer_to_value_to_set, uavcan_protocol_param_Value_type_t data_type) {
    switch (data_type) {
        case UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE:
            return set_parameter_value_by_name(name, *(bool*)pointer_to_value_to_set);
        case UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE:
            return set_parameter_value_by_name(name, *(int64_t*)pointer_to_value_to_set);
        case UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE:
            return set_parameter_value_by_name(name, *(float*)pointer_to_value_to_set);
        default:
            return false;
    }
}

uavcan_parameter DroneCAN_service::get_parameter_by_name(const char* name) {
    auto iterator = parameter_list.begin();
    while(iterator != parameter_list.end()) {
        if (strcmp((char*)iterator->name.data, name) == 0)
            return *iterator;
        ++iterator;
    }
    uavcan_parameter invalid_parameter;
    strcpy((char*)invalid_parameter.name.data, NAME_FOR_INVALID_PARAMETER);
    return invalid_parameter;
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

bool DroneCAN_service::set_parameter_value_by_name(const char* name, bool value_to_set) {
    return set_generic_parameter_value_by_name(name, value_to_set);
}

bool DroneCAN_service::set_parameter_value_by_name(const char* name, int32_t value_to_set) {
    return set_generic_parameter_value_by_name(name, value_to_set);
}

bool DroneCAN_service::set_parameter_value_by_name(const char* name, int64_t value_to_set) {
    return set_generic_parameter_value_by_name(name, value_to_set);
}

bool DroneCAN_service::set_parameter_value_by_name(const char* name, float value_to_set) {
    return set_generic_parameter_value_by_name(name, value_to_set);
}

bool DroneCAN_service::set_parameter_value_by_name(const char* name, char* value_to_set) {
    return set_generic_parameter_value_by_name(name, value_to_set);
}

template <typename PARAM_VALUE_TYPE>
bool DroneCAN_service::set_generic_parameter_value_by_name(const char* name, PARAM_VALUE_TYPE value_to_set) {
    auto iterator = parameter_list.begin();
    while (iterator != parameter_list.end()) {
        if (strcmp((char*)iterator->name.data, name) == 0) {
            iterator->value = package_uavcan_param_value(value_to_set);
            return true;
        }
        ++iterator;
    }
    return false;
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, bool value_to_set) {
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, int32_t value_to_set) {
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, int64_t value_to_set) { //TODO: add test
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, float value_to_set) {
    return set_generic_parameter_value(parameter_index_from_0, value_to_set);
}

bool DroneCAN_service::set_parameter_value(uint8_t parameter_index_from_0, char* value_to_set) {
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