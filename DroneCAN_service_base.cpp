#include "DroneCAN_service_base.h"

void DroneCAN_service_base::publish_generic_message(canard_message_type_info_t type_info, canard_message_data_t data) {
    try_broadcast_with_canard(type_info, data);
    send_pending_CAN_frames();
}

void DroneCAN_service_base::try_initialize_CAN_bus_driver() {
    can_driver.setPins(CAN_BUS_RX_PIN, CAN_BUS_TX_PIN);
    _is_healthy = can_driver.begin(CAN_BUS_BAUDRATE);
    if (!_is_healthy)
        _handle_error(DroneCAN_error::ON_INITIALIZATION);
}

void DroneCAN_service_base::try_broadcast_with_canard(canard_message_type_info_t type_info, canard_message_data_t data) {
    _is_healthy = canard.broadcast(type_info, data) >= 0;
    if(!_is_healthy)
        _handle_error(DroneCAN_error::FAIL_ON_PUBLISH);
}

void DroneCAN_service_base::send_pending_CAN_frames() {
    while (!canard.is_txQueue_empty()){
        CanardCANFrame* frame_to_send = (CanardCANFrame*)canard.peekTxQueue();
        try_send_CAN_bus_frame(*frame_to_send);
    }
}

void DroneCAN_service_base::try_send_CAN_bus_frame(CanardCANFrame& frame) {
    bool _is_healthy = can_driver.send_frame(frame);
    if (!_is_healthy)
        _handle_error(DroneCAN_error::FAIL_ON_PUBLISH);
}