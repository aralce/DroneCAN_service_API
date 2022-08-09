#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <canard.h>
#include <uavcan.protocol.param.Empty.h>

enum uavcan_protocol_param_Value_type_t {
    UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY,
    UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE,
    UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE,
    UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE,
    UAVCAN_PROTOCOL_PARAM_VALUE_STRING_VALUE,
};

struct uavcan_protocol_param_Value {
    enum uavcan_protocol_param_Value_type_t union_tag;
    union {
        struct uavcan_protocol_param_Empty empty;
        int64_t integer_value;
        float real_value;
        uint8_t boolean_value;
        struct { uint8_t len; uint8_t data[128]; }string_value;
    };
};