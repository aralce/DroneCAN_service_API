#pragma once

#include <canard.h>
#include <Canard_wrapper.h>
#include <uavcan.equipment.power.BatteryInfo.h>
#include <uavcan.protocol.NodeStatus.h>
#include <uavcan.protocol.param.GetSet_res.h>

class DSDL_to_canard_DTO {
public:
    canard_message_type_info_t get_type_info();
    canard_message_data_t get_data();

    DSDL_to_canard_DTO(uavcan_equipment_power_BatteryInfo& battery_info);
    DSDL_to_canard_DTO(uavcan_protocol_NodeStatus& node_status);
    DSDL_to_canard_DTO(uavcan_protocol_param_GetSetResponse& param_response);
};