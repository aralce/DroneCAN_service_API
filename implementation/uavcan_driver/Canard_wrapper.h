#pragma once
#include "canard.h"
#include <cstring>
#include "../../DroneCAN_service_configuration.h"

typedef struct {
    uint64_t signature;
    uint16_t id;
    uint8_t priority;
}canard_message_type_info_t;

typedef struct {
    void* ptr;
    uint16_t length;
}canard_message_data_t;

static uint8_t canard_buffer[LIBCANARD_ALLOCATION_BUFFER_IN_BYTES];
static uint8_t canard_transferID;

class Canard {
public:
    void init() {
        init(handle_reception_DUMMY, handle_acceptance_DUMMY);
    }
    
    void init(CanardOnTransferReception handle_reception, CanardShouldAcceptTransfer handle_acceptance) {
        canardInit(&canard_instance,
                   canard_buffer,
                   buffer_size,
                   handle_reception,
                   handle_acceptance,
                   NULL);
    }

    void set_node_ID(uint8_t self_node_id) {
        canard_instance.node_id = self_node_id;
    }
    
    int16_t broadcast(canard_message_type_info_t& type_info, canard_message_data_t& data) {
        return canardBroadcast(&canard_instance,
                               type_info.signature, 
                               type_info.id,
                               &canard_transferID,
                               type_info.priority,
                               data.ptr,
                               data.length);
    }

    int16_t send_response(uint8_t destination_node_id, canard_message_type_info_t& type_info, canard_message_data_t& data) {
        return canardRequestOrRespond(&canard_instance,
                                      destination_node_id,
                                      type_info.signature,
                                      type_info.id,
                                      &canard_transferID,
                                      type_info.priority,
                                      CanardResponse,
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

    int16_t handle_rx_frame(CanardCANFrame& frame, uint64_t timestamp_usec) {
        memcpy(&this->frame, &frame, sizeof(CanardCANFrame));
        this->frame.id |= CANARD_CAN_FRAME_EFF;
        return canardHandleRxFrame(&canard_instance, &(this->frame), timestamp_usec);
    }

    void release_rx_memory(CanardRxTransfer* rx_transfer) {
        canardReleaseRxTransferPayload(&canard_instance, rx_transfer);
    }

    void clean(uint64_t timestamp_usec) {
        canardCleanupStaleTransfers(&canard_instance, timestamp_usec);
    }

    CanardPoolAllocatorStatistics get_pool_allocator_statistics() {
        return canardGetPoolAllocatorStatistics(&canard_instance);
    }
    
private:
    friend class Canard_spy;
    CanardInstance canard_instance;
    CanardCANFrame frame{};
    static constexpr size_t buffer_size = LIBCANARD_ALLOCATION_BUFFER_IN_BYTES; 
      
    static void handle_reception_DUMMY(CanardInstance*, CanardRxTransfer*) {};
    static bool handle_acceptance_DUMMY(const CanardInstance*, uint64_t*, uint16_t, CanardTransferType, uint8_t) {return false;};
};