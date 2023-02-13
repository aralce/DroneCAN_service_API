#include "Canard_wrapper.h"
#include <CAN_BUS_adaptor/Can_bus_adaptor.h>

Canard::Canard(uint32_t bytes_allocation_buffer, uint32_t max_bytes_on_message) {}

void Canard::init() {
    mock().actualCall("canard->init");
}

CanardOnTransferReception Canard::on_reception_ptr = nullptr;
CanardShouldAcceptTransfer Canard::should_accept_ptr = nullptr;

void Canard::init(CanardOnTransferReception handle_reception, CanardShouldAcceptTransfer handle_acceptance) {
    on_reception_ptr = handle_reception;
    should_accept_ptr = handle_acceptance;

    mock().actualCall("canard->init_with_reception_handler")
          .withPointerParameter("handle_reception", (void*)handle_reception)
          .withPointerParameter("handle_acceptance", (void*)handle_acceptance);
}

CanardOnTransferReception Canard::spy_handle_reception_function_pointer() {
    return on_reception_ptr;
}

CanardShouldAcceptTransfer Canard::spy_handle_acceptance_function_pointer() {
    return should_accept_ptr;
}

void Canard::set_node_ID(uint8_t self_node_id) {
    mock().actualCall("canard->set_node_ID")
          .withUnsignedIntParameter("self_node_id", self_node_id);
}

int16_t Canard::broadcast(canard_message_type_info_t& type_info, canard_message_data_t& data) {
    mock().actualCall("canard->broadcast")
          .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&type_info)
          .withParameterOfType("canard_message_data_t", "data", (const void*)&data);
    return mock().returnIntValueOrDefault(0);
}

int16_t Canard::send_response(uint8_t destination_node_id, canard_message_type_info_t& type_info, canard_message_data_t& data) {
    mock().actualCall("canard->send_response")
          .withUnsignedIntParameter("destination_node_id", destination_node_id)
          .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&type_info)
          .withParameterOfType("canard_message_data_t", "data", (const void*)&data);
    return mock().returnIntValueOrDefault(0);
}

const CanardCANFrame* Canard::peekTxQueue() {
    mock().actualCall("canard->peekTxQueue");
    return (CanardCANFrame*)mock().returnPointerValueOrDefault(nullptr);     
}

void Canard::popTxQueue() {
    mock().actualCall("canard->popTxQueue");
}

bool Canard::is_txQueue_empty() {
    mock().actualCall("canard->is_txQueue_empty");
    return mock().returnBoolValueOrDefault(true);              
}

int16_t Canard::handle_rx_frame(const CanardCANFrame& frame, uint64_t timestamp_usec) {
    mock().actualCall("canard->handle_rx_frame")
          .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&frame)
          .withUnsignedLongLongIntParameter("timestamp_usec", timestamp_usec);
    return mock().returnIntValueOrDefault(0);
}