#include "DroneCAN_message_sender.h"
#include <DSDL_to_canard_DTO.h>

DroneCAN_message_sender::DroneCAN_message_sender(uint8_t node_ID, droneCAN_handle_error_t handle_error)
  : _handle_error(handle_error), _node_ID(node_ID)
{

}

void DroneCAN_message_sender::broadcast_message(uavcan_equipment_power_BatteryInfo& battery_info) {    
    DSDL_to_canard_DTO data_transfer_object(battery_info);
    
    canard_message_type_info_t message_type_info =  data_transfer_object.get_type_info();
    canard_message_data_t data = data_transfer_object.get_data();
    try_broadcast_with_canard(message_type_info, data);
    
    while(!canard.is_txQueue_empty()) {
        CanardCANFrame* frame_to_send = (CanardCANFrame*)canard.peekTxQueue();
        canard.popTxQueue();
        can_driver.send_frame(*frame_to_send);
    }
}

void DroneCAN_message_sender::try_broadcast_with_canard(canard_message_type_info_t& type_info, canard_message_data_t& data) {
    _is_healthy = canard.broadcast(type_info, data) >= 0;
    if(!_is_healthy)
        _handle_error(DroneCAN_error::FAIL_ON_PUBLISH);
}

///////
void DroneCAN_message_sender::publish_generic_message(canard_message_type_info_t type_info, canard_message_data_t data) {
    try_broadcast_with_canard(type_info, data);
    send_pending_CAN_frames();
}

//This doesn't goes here
void DroneCAN_message_sender::try_initialize_CAN_bus_driver() {
    can_driver.setPins(CAN_BUS_CRX_PIN, CAN_BUS_CTX_PIN);
    _is_healthy = can_driver.begin(CAN_BUS_BAUDRATE);
    if (!_is_healthy)
        _handle_error(DroneCAN_error::ON_INITIALIZATION);
}


void DroneCAN_message_sender::send_pending_CAN_frames() {
    while (!canard.is_txQueue_empty()){
        CanardCANFrame* frame_to_send = (CanardCANFrame*)canard.peekTxQueue();
        canard.popTxQueue();
        try_send_CAN_bus_frame(*frame_to_send);
    }
}

void DroneCAN_message_sender::try_send_CAN_bus_frame(CanardCANFrame& frame) {
    bool _is_healthy = can_driver.send_frame(frame);
    if (!_is_healthy)
        _handle_error(DroneCAN_error::FAIL_ON_PUBLISH);
}