#pragma once
#include "common_to_all_mocks.h"

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
        mock().actualCall("init");
    }
      
    void broadcast(canard_message_type_info_t type_info, canard_message_data_t data) {
        mock().actualCall("broadcast")
              .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&type_info)
              .withParameterOfType("canard_message_data_t", "data", (const void*)&data);
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
        if (!are_data_pointer_values_equal(data_1_pointer, data_2_pointer, data_1->length))
            return false;
        
        return true;
    }
    virtual SimpleString valueToString(const void* type_info)
    {
        return StringFrom(type_info);
    }

private:
    bool are_data_pointer_values_equal(uint8_t* ptr_1, uint8_t* ptr_2, uint16_t data_length) {
        for(int element=0; element<data_length; ++element, ++ptr_1, ++ptr_2)
            if(*ptr_1 != *ptr_2)
                return false;
        
        return true;
    }
};
