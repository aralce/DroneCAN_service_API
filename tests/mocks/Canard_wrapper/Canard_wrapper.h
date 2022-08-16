#pragma once
#include "common_to_all_mocks.h"
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

class Canard {
public:
    explicit Canard(uint32_t bytes_allocation_buffer, uint32_t max_bytes_on_message);

    void init();

    void init(CanardOnTransferReception handle_reception, CanardShouldAcceptTransfer handle_acceptance);
    static CanardOnTransferReception spy_handle_reception_function_pointer();
    static CanardShouldAcceptTransfer spy_handle_acceptance_function_pointer();

    void set_node_ID(uint8_t self_node_id);
      
    int16_t broadcast(canard_message_type_info_t& type_info, canard_message_data_t& data);

    int16_t send_response(uint8_t destination_node_id, canard_message_type_info_t& type_info, canard_message_data_t& data);

    const CanardCANFrame* peekTxQueue();

    void popTxQueue();

    bool is_txQueue_empty();

    int16_t handle_rx_frame(const CanardCANFrame& frame, uint64_t timestamp_usec);

private:
    static CanardOnTransferReception on_reception_ptr;
    static CanardShouldAcceptTransfer should_accept_ptr;
};

class Canard_message_type_info_t_comparator : public MockNamedValueComparator
{
public:
    virtual bool isEqual(const void* object1, const void* object2)
    {
        canard_message_type_info_t* type_info_1 = (canard_message_type_info_t*)object1;
        canard_message_type_info_t* type_info_2 = (canard_message_type_info_t*)object2;

        if (type_info_1->id != type_info_2->id)
            return false;
        if (type_info_1->priority != type_info_2->priority)
            return false;
        if (type_info_1->signature != type_info_2->signature)
            return false;
        return true;
    }

    virtual SimpleString valueToString(const void* type_info)
    {
        return StringFrom(type_info);
    }
};

class Canard_message_data_t_comparator : public MockNamedValueComparator
{
public:
    virtual bool isEqual(const void* object1, const void* object2)
    {
        canard_message_data_t* data_1 = (canard_message_data_t*)object1;
        canard_message_data_t* data_2 = (canard_message_data_t*)object2;

        if (data_1->length != data_2->length)
            return false;
        
        uint8_t* data_1_pointer = (uint8_t*)data_1->ptr;
        uint8_t* data_2_pointer = (uint8_t*)data_2->ptr;
        if (!are_data_pointer_equal(data_1_pointer, data_2_pointer, data_1->length))
            return false;
        
        return true;
    }
    virtual SimpleString valueToString(const void* data)
    {
        return StringFrom(data);
    }
};
