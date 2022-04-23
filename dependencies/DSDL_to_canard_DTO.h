#pragma once
#ifdef IS_RUNNING_TESTS
    #include <canard.h>
    #include <Canard_wrapper.h>
    #include <uavcan.equipment.power.BatteryInfo.h>
    #include <uavcan.protocol.NodeStatus.h>
#else
    #include "canard.h"
    #include "Canard_wrapper.h"
    #include "dependencies/dsdlc_generated/include/uavcan/uavcan.equipment.power.BatteryInfo.h"
    #include "dependencies/dsdlc_generated/include/uavcan/uavcan.protocol.NodeStatus.h"
#endif
#include "../DroneCAN_service_configuration.h"

typedef enum {NODE_STATUS, BATTERY_INFO, NUMBER_OF_MESSAGES}type_of_message;
class DSDL_to_canard_DTO {
private:
    uint8_t buffer[UAVCAN_MAX_BYTES_ON_MESSAGE]{};
    canard_message_type_info_t type_info;
    canard_message_data_t data;

public:
    canard_message_type_info_t& get_type_info() {
        return type_info;
    }

    canard_message_data_t& get_data() {
        return data;
    }

    DSDL_to_canard_DTO(uavcan_equipment_power_BatteryInfo& battery_info);
    DSDL_to_canard_DTO(uavcan_protocol_NodeStatus& node_status);
};