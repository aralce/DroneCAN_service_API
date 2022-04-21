#include "DroneCAN_service_API.h"
#include "DroneCAN_service_configuration.h"

DroneCAN_service::DroneCAN_service(droneCAN_handle_error_t handle_error) : DroneCAN_service(DEFAULT_NODE_ID, handle_error) {

}

DroneCAN_service::DroneCAN_service(uint8_t node_ID, droneCAN_handle_error_t handle_error)
    : DroneCAN_service_base(node_ID, handle_error) 
{
    canard.set_node_ID(node_ID);
    canard.init();
    try_initialize_CAN_bus_driver();
}

void DroneCAN_service::publish_regularly(get_battery_info_t get_message, uint32_t milliseconds_between_publish) {
    if (milliseconds_between_publish == 0) {
        _get_battery_info = nullptr;
        return;
    }
    _get_battery_info = get_message;
    uavcan_equipment_power_BatteryInfo battery_info = _get_battery_info();
    publish_message(battery_info);
    _time_between_publishes = milliseconds_between_publish;
}

void DroneCAN_service::publish_message(uavcan_equipment_power_BatteryInfo& battery_info) {
    uint8_t buffer[UAVCAN_EQUIPMENT_POWER_BATTERYINFO_MAX_SIZE]{};
    uint32_t message_length = uavcan_equipment_power_BatteryInfo_encode(&battery_info, buffer);
    
    canard_message_type_info_t type_info = {.signature = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_SIGNATURE,
                                            .id = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_ID,
                                            .priority = CANARD_TRANSFER_PRIORITY_LOW };
    canard_message_data_t data = {.ptr = (void*)buffer, .length = (uint16_t)message_length };
    try_broadcast_with_canard(type_info, data);
    
    send_pending_CAN_frames();
}

uint8_t DroneCAN_service::get_node_ID() {
    return _node_ID;
}

bool DroneCAN_service::is_healthy() {
    return _is_healthy;
}