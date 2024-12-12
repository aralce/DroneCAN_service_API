#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <cstring>
#include <canard.h>
#include <common_to_all_mocks.h>
#include <uavcan.protocol.param.NumericValue.h>
#include <uavcan.protocol.param.Value.h>

#define UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_MAX_SIZE 371
#define UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_SIGNATURE (0xA7B622F939D1A4D5ULL)
#define UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_ID 11

struct uavcan_protocol_param_GetSetResponse {
    struct uavcan_protocol_param_Value value;
    struct uavcan_protocol_param_Value default_value;
    struct uavcan_protocol_param_NumericValue max_value;
    struct uavcan_protocol_param_NumericValue min_value;
    struct { uint8_t len; uint8_t data[92]; }name;
};

uint32_t uavcan_protocol_param_GetSetResponse_encode(struct uavcan_protocol_param_GetSetResponse* msg, uint8_t* buffer);

class Uavcan_protocol_param_GetSetResponse_comparator : public MockNamedValueComparator
{
public:
    virtual bool isEqual(const void* object1, const void* object2)
    {
        uavcan_protocol_param_GetSetResponse* msg_1 = (uavcan_protocol_param_GetSetResponse*)object1;
        uavcan_protocol_param_GetSetResponse* msg_2 = (uavcan_protocol_param_GetSetResponse*)object2;

        if (msg_1->name.len != msg_2->name.len) {
            printf("name_len mismatch:  len1: %u | len2: %u\n", msg_1->name.len, msg_2->name.len);
            return false;
        }
        
        #define STRINGS_ARE_EQUAL 0
        if (strcmp((const char*)msg_1->name.data, (const char*)msg_2->name.data) != STRINGS_ARE_EQUAL) {
            printf("name mismatch:\n");
            return false;
        }

        if (msg_1->value.union_tag != msg_2->value.union_tag) {
            printf("value-union_tag mismatch\n");
            return false;
        }
        if (msg_1->value.integer_value != msg_2->value.integer_value) {
            printf("value-integer_value mismatch: value_1->%ld | value_2->%ld\n", msg_1->value.integer_value, msg_2->value.integer_value);
            return false;
        }

        if (msg_1->default_value.union_tag != msg_2->default_value.union_tag)
            return false;
        if (msg_1->default_value.integer_value != msg_2->default_value.integer_value)
            return false;

        if (msg_1->min_value.union_tag != msg_2->min_value.union_tag)
            return false;
        if (msg_1->min_value.integer_value != msg_2->min_value.integer_value)
            return false;

        if (msg_1->max_value.union_tag != msg_2->max_value.union_tag)
            return false;
        if (msg_1->max_value.integer_value != msg_2->max_value.integer_value)
            return false;

        return true;
    }

    virtual SimpleString valueToString(const void* type_info)
    {
        return StringFrom(type_info);
    }
};