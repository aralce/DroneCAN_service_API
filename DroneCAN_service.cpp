#include "DroneCAN_service.h"
#include "DroneCAN_service_configuration.h"

DroneCAN_service::DroneCAN_service(droneCAN_handle_error_t handle_error) : _handle_error(handle_error) {
    canard.init();
    try_initialize_CAN_bus_driver();
}

void DroneCAN_service::try_initialize_CAN_bus_driver() {
    can_driver.setPins(CAN_BUS_RX_PIN, CAN_BUS_TX_PIN);
    _is_healthy = can_driver.begin(CAN_BUS_BAUDRATE);
    if (!_is_healthy)
        _handle_error(DroneCAN_error::ON_INITIALIZATION);
}


bool DroneCAN_service::is_healthy() {
    return _is_healthy;
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

void DroneCAN_service::try_broadcast_with_canard(canard_message_type_info_t& type_info, canard_message_data_t data) {
    _is_healthy = canard.broadcast(type_info, data) >=0;
    if(!_is_healthy)
        _handle_error(DroneCAN_error::FAIL_ON_PUBLISH);
}

void DroneCAN_service::send_pending_CAN_frames() {
    while (!canard.is_txQueue_empty()){
        CanardCANFrame* frame_to_send = (CanardCANFrame*)canard.canardPeekTxQueue();
        try_send_CAN_bus_frame(*frame_to_send);
    }
}

void DroneCAN_service::try_send_CAN_bus_frame(CanardCANFrame& frame) {
    bool _is_healthy = can_driver.send_frame(frame);
    if (!_is_healthy)
        _handle_error(DroneCAN_error::FAIL_ON_PUBLISH);
}