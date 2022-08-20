#include "DroneCAN_message_sender.h"

static void dummy_function(DroneCAN_error error) {}

DroneCAN_message_sender::DroneCAN_message_sender(Canard& canard, CAN_bus_adaptor& can_driver, droneCAN_handle_error_t handle_error)
  : _canard(canard), _can_driver(can_driver)
{
    _handle_error = handle_error == nullptr ? dummy_function : handle_error;
}

void DroneCAN_message_sender::broadcast_message(DSDL_to_canard_DTO& data_transfer_object) {    
    canard_message_type_info_t message_type_info =  data_transfer_object.get_type_info();
    canard_message_data_t data = data_transfer_object.get_data();
    try_broadcast_with_canard(message_type_info, data);
    
    send_pending_CAN_frames();
}

void DroneCAN_message_sender::try_broadcast_with_canard(canard_message_type_info_t& type_info, canard_message_data_t& data) {
    _is_healthy = _canard.broadcast(type_info, data) >= 0;
    if(!_is_healthy)
        _handle_error(DroneCAN_error::FAIL_ON_PUBLISH);
}

void DroneCAN_message_sender::send_response_message(DSDL_to_canard_DTO& data_transfer_object, uint8_t destination_node_id) {
    canard_message_type_info_t message_type_info = data_transfer_object.get_type_info();
    canard_message_data_t data = data_transfer_object.get_data();
    try_send_response_message_with_canard(destination_node_id, message_type_info, data);
    send_pending_CAN_frames();
}

// void DroneCAN_message_sender::send_response_message(uavcan_protocol_param_GetSetResponse& param_response, uint8_t destination_node_id) {
//     DSDL_to_canard_DTO data_transfer_object(param_response);
//     canard_message_type_info_t message_type_info = data_transfer_object.get_type_info();
//     canard_message_data_t data = data_transfer_object.get_data();
//     try_send_response_message_with_canard(destination_node_id, message_type_info, data);
//     send_pending_CAN_frames();
// }

void DroneCAN_message_sender::try_send_response_message_with_canard(uint8_t destination_node_id, canard_message_type_info_t& type_info, canard_message_data_t& data)
{
    _is_healthy = _canard.send_response(destination_node_id, type_info, data) >= 0;
    if (!_is_healthy)
        _handle_error(DroneCAN_error::FAIL_ON_PUBLISH);
}

void DroneCAN_message_sender::send_pending_CAN_frames() {
    while (!_canard.is_txQueue_empty()){
        CanardCANFrame* frame_to_send = (CanardCANFrame*)_canard.peekTxQueue();
        _canard.popTxQueue();
        try_send_CAN_bus_frame(*frame_to_send);
    }
}

void DroneCAN_message_sender::try_send_CAN_bus_frame(CanardCANFrame& frame) {
    _is_healthy = _can_driver.send_frame(frame);
    if (!_is_healthy)
        _handle_error(DroneCAN_error::FAIL_ON_PUBLISH);
}
