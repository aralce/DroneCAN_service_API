#include <DSDL_to_canard_DTO.h>
#include <common_to_all_mocks.h>

canard_message_type_info_t DSDL_to_canard_DTO::get_type_info() {
    mock().actualCall("DSDL_to_canard_DTO->get_type_info");
    canard_message_type_info_t type_info;
    return *(canard_message_type_info_t*)mock().returnPointerValueOrDefault((void*)&type_info);
}

canard_message_data_t DSDL_to_canard_DTO::get_data() {
    mock().actualCall("DSDL_to_canard_DTO->get_data");
    canard_message_data_t data;
    return *(canard_message_data_t*)mock().returnPointerValueOrDefault((void*)&data);
}

DSDL_to_canard_DTO::DSDL_to_canard_DTO(uavcan_equipment_power_BatteryInfo& battery_info) {
    mock().actualCall("DSDL_to_canard_DTO->construct_with_uavcan_power_BatteryInfo");
}

DSDL_to_canard_DTO::DSDL_to_canard_DTO(uavcan_protocol_NodeStatus& node_status) {
    mock().actualCall("DSDL_to_canard_DTO->constuct_with_uavcan_protocol_NodeStatus");
}

DSDL_to_canard_DTO::DSDL_to_canard_DTO(uavcan_protocol_param_GetSetResponse& param_response) {
    mock().actualCall("DSDL_to_canard_DTO->construct_with_uavcan_protocol_param_GetSetResponse");
}

DSDL_to_canard_DTO::DSDL_to_canard_DTO(uavcan_protocol_GetNodeInfoResponse& get_node_info_response) {
    mock().actualCall("DSDL_to_canard_DTO->construct_with_uavcan_protocol_GetNodeInfoResponse");
}