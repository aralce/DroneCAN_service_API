#pragma once
#include <canard.h>

typedef struct {
    uint64_t signature;
    uint16_t id;
    uint8_t priority;
}canard_message_type_info_t;

typedef struct {
    void* ptr;
    uint16_t length;
}canard_message_data_t;

template <int ALLOCATION_BUFFER_IN_BYTES, int MAX_BYTES_ON_MESSAGE>
class Canard {
public:
    void init() {
        init(handle_reception_DUMMY, handle_acceptance_DUMMY);
    }
    
    void init(CanardOnTransferReception handle_reception, CanardShouldAcceptTransfer handle_acceptance) {
        canardInit(&this->canard_instance,
                   this->canard_buffer,
                   ALLOCATION_BUFFER_IN_BYTES,
                   handle_reception,
                   handle_acceptance,
                   NULL);
    }

    void set_node_ID(uint8_t self_node_id) {
        canardSetLocalNodeID(&canard_instance, self_node_id);
    }
    
    int16_t broadcast(canard_message_type_info_t type_info, canard_message_data_t data) {
        return canardBroadcast(&this->canard_instance,
                               type_info.signature, 
                               type_info.id,
                               &this->canard_transferID,
                               type_info.priority,
                               data.ptr,
                               data.length);
    }

    const CanardCANFrame* peekTxQueue() {
        const CanardCANFrame* frame_to_send = canardPeekTxQueue((const CanardInstance*)&canard_instance);
        return frame_to_send;
    }

    void popTxQueue() {
        canardPopTxQueue(&canard_instance);
    }

    bool is_txQueue_empty() {
        if(canardPeekTxQueue((const CanardInstance*)&canard_instance) == NULL)
            return true;
        return false;
    }
    
private:
    CanardInstance canard_instance;
    uint8_t canard_buffer[ALLOCATION_BUFFER_IN_BYTES];
    uint8_t canard_transferID;
    uint8_t canard_transmission_buffer[MAX_BYTES_ON_MESSAGE];
      
    static void handle_reception_DUMMY(CanardInstance*, CanardRxTransfer*) {};
    static bool handle_acceptance_DUMMY(const CanardInstance*, uint64_t*, uint16_t, CanardTransferType, uint8_t) {return false;};
};