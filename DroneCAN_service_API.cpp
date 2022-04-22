#include "DroneCAN_service_API.h"
#include "DroneCAN_service_configuration.h"
#include <DSDL_to_canard_DTO.h>

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
    DSDL_to_canard_DTO data_transfer_object(battery_info);
    try_broadcast_with_canard(data_transfer_object.get_type_info(), data_transfer_object.get_data());
    
    send_pending_CAN_frames();
}

void DroneCAN_service::publish_message(uavcan_protocol_NodeStatus& node_status) {
    DSDL_to_canard_DTO data_transfer_object(node_status);
    try_broadcast_with_canard(data_transfer_object.get_type_info(), data_transfer_object.get_data());

    send_pending_CAN_frames();
}

uint8_t DroneCAN_service::get_node_ID() {
    return _node_ID;
}

bool DroneCAN_service::is_healthy() {
    return _is_healthy;
}