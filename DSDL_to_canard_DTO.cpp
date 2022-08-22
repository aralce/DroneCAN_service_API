#include "DSDL_to_canard_DTO.h"
#include <Arduino.h>

DSDL_to_canard_DTO::DSDL_to_canard_DTO(uavcan_equipment_power_BatteryInfo& battery_info) {
    uint32_t message_length = uavcan_equipment_power_BatteryInfo_encode(&battery_info, buffer);
    
    type_info.signature = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_SIGNATURE;
    type_info.id = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_ID;
    type_info.priority = CANARD_TRANSFER_PRIORITY_MEDIUM;
    
    data.ptr = (void*)buffer;
    data.length = (uint16_t)message_length;
}

DSDL_to_canard_DTO::DSDL_to_canard_DTO(uavcan_protocol_NodeStatus& node_status) {
    uint32_t message_length = uavcan_protocol_NodeStatus_encode(&node_status, buffer);

    type_info.signature = UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE;
    type_info.id = UAVCAN_PROTOCOL_NODESTATUS_ID;
    type_info.priority = CANARD_TRANSFER_PRIORITY_HIGH;
    
    data.ptr = (void*)buffer;
    data.length = (uint16_t)message_length;
}

DSDL_to_canard_DTO::DSDL_to_canard_DTO(uavcan_protocol_param_GetSetResponse param_response) {
    uint32_t message_length = uavcan_protocol_param_GetSetResponse_encode(&param_response, buffer);
    type_info.signature = UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_SIGNATURE;
    type_info.id = UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_ID;
    type_info.priority = CANARD_TRANSFER_PRIORITY_MEDIUM;

    data.ptr = (void*)buffer;
    data.length = (uint16_t)message_length;
}

DSDL_to_canard_DTO::DSDL_to_canard_DTO(uavcan_protocol_GetNodeInfoResponse get_node_info_response) {
    uint32_t message_length = uavcan_protocol_GetNodeInfoResponse_encode(&get_node_info_response, buffer);
    type_info.signature = UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_SIGNATURE;
    type_info.id = UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_ID;
    type_info.priority = CANARD_TRANSFER_PRIORITY_HIGH;

    data.ptr = (void*)buffer;
    data.length = (uint16_t)message_length;
}