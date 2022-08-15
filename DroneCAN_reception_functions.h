#pragma once
#include <canard.h>

void handle_received_droneCAN_message(CanardInstance* ins, CanardRxTransfer* transfer);

bool should_accept_droneCAN_message(const CanardInstance* ins, uint64_t* out_data_type_signature, uint16_t data_type_id, CanardTransferType transfer_type, uint8_t source_node_id);