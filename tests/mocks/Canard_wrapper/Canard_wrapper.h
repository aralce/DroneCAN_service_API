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
    explicit Canard(uint32_t bytes_allocation_buffer, uint32_t max_bytes_on_message) {}

    void init() {
        mock().actualCall("canard->init");
    }

    void init(CanardOnTransferReception handle_reception, CanardShouldAcceptTransfer handle_acceptance) {
        mock().actualCall("canard->init_with_reception_handler")
              .withPointerParameter("handle_reception", (void*)handle_reception)
              .withPointerParameter("handle_acceptance", (void*)handle_acceptance);
    }

    void set_node_ID(uint8_t self_node_id) {
        mock().actualCall("canard->set_node_ID")
              .withUnsignedIntParameter("self_node_id", self_node_id);
    }
      
    int16_t broadcast(canard_message_type_info_t& type_info, canard_message_data_t& data) {
        mock().actualCall("canard->broadcast")
              .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&type_info)
              .withParameterOfType("canard_message_data_t", "data", (const void*)&data);
        return mock().returnIntValueOrDefault(0);
    }

    int16_t send_response(uint8_t destination_node_id, canard_message_type_info_t& type_info, canard_message_data_t& data) {
        mock().actualCall("canard->send_response")
              .withUnsignedIntParameter("destination_node_id", destination_node_id)
              .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&type_info)
              .withParameterOfType("canard_message_data_t", "data", (const void*)&data);
        return mock().returnIntValueOrDefault(0);
    }

    const CanardCANFrame* peekTxQueue() {
        mock().actualCall("canard->peekTxQueue");
        return (CanardCANFrame*)mock().returnPointerValueOrDefault(nullptr);     
    }

    void popTxQueue() {
        mock().actualCall("canard->popTxQueue");
    }

    bool is_txQueue_empty() {
        mock().actualCall("canard->is_txQueue_empty");
        return mock().returnBoolValueOrDefault(true);              
    }
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
