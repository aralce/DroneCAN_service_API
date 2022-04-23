#pragma once
#include "canard.h"

typedef struct {
    uint64_t signature;
    uint16_t id;
    uint8_t priority;
}canard_message_type_info_t;

typedef struct {
    void* ptr;
    uint16_t length;
}canard_message_data_t;

template <int LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, int UAVCAN_MAX_BYTES_ON_MESSAGE>
class Canard {
public:
    void init() {
        init(handle_reception_DUMMY, handle_acceptance_DUMMY);
    }
    
    void init(CanardOnTransferReception handle_reception, CanardShouldAcceptTransfer handle_acceptance) {
        canardInit(&canard_instance,
                   canard_buffer,
                   LIBCANARD_ALLOCATION_BUFFER_IN_BYTES,
                   handle_reception,
                   handle_acceptance,
                   NULL);
    }

    void set_node_ID(uint8_t self_node_id) {
        canardSetLocalNodeID(&canard_instance, self_node_id);
    }
    
    int16_t broadcast(canard_message_type_info_t type_info, canard_message_data_t data) {
        return canardBroadcast(&canard_instance,
                               type_info.signature, 
                               type_info.id,
                               &canard_transferID,
                               type_info.priority,
                               data.ptr,
                               data.length);
    }

    const CanardCANFrame* peekTxQueue() {
        const CanardCANFrame* frame_to_send = canardPeekTxQueue((const CanardInstance*)&canard_instance);
        _is_txQueue_empty = (frame_to_send == NULL);
        return frame_to_send;
    }

    bool is_txQueue_empty() {
        return _is_txQueue_empty;
    }
    
private:
    CanardInstance canard_instance;
    uint8_t canard_buffer[LIBCANARD_ALLOCATION_BUFFER_IN_BYTES];
    uint8_t canard_transferID;
    uint8_t canard_transmission_buffer[UAVCAN_MAX_BYTES_ON_MESSAGE];
    
    bool _is_txQueue_empty = true;
    
    static void handle_reception_DUMMY(CanardInstance*, CanardRxTransfer*) {};
    static bool handle_acceptance_DUMMY(const CanardInstance*, uint64_t*, uint16_t, CanardTransferType, uint8_t) {return false;};
};