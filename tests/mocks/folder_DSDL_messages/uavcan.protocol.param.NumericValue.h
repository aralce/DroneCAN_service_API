#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "../Canard_wrapper/Canard/canard.h"
#include "uavcan.protocol.param.Empty.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_MAX_SIZE 9
#define UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_SIGNATURE (0xDA6D6FEA22E3587ULL)

enum uavcan_protocol_param_NumericValue_type_t {
    UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_EMPTY,
    UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE,
    UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE,
};

struct uavcan_protocol_param_NumericValue {
    enum uavcan_protocol_param_NumericValue_type_t union_tag;
    union {
        struct uavcan_protocol_param_Empty empty;
        int64_t integer_value;
        float real_value;
    };
};

#ifdef __cplusplus
}
#endif