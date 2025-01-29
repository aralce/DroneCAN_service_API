#include "auxiliary_functions.h"
#include <cstdio> 

uavcan_protocol_param_Value package_uavcan_param_value(int32_t value_to_package) {
    return package_uavcan_param_value((int64_t)value_to_package);
}

uavcan_protocol_param_Value package_uavcan_param_value(int64_t value_to_package) {
    uavcan_protocol_param_Value param_value{};
    param_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
    param_value.integer_value = value_to_package;
    return param_value;
}

uavcan_protocol_param_Value package_uavcan_param_value(float value_to_package) {
    uavcan_protocol_param_Value param_value{};
    param_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;
    param_value.real_value = value_to_package;
    return param_value;
}

uavcan_protocol_param_Value package_uavcan_param_value(bool value_to_package) {
    uavcan_protocol_param_Value param_value{};
    param_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE;
    param_value.boolean_value = value_to_package;
    return param_value;
}

uavcan_protocol_param_Value package_uavcan_param_value(const uint8_t* value_to_package) {
    return package_uavcan_param_value((const char*)value_to_package);
}

uavcan_protocol_param_Value package_uavcan_param_value(const char* value_to_package) {
    uavcan_protocol_param_Value param_value{};
    param_value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_STRING_VALUE;
    param_value.string_value.len = strlen(value_to_package);
    strncpy((char*)param_value.string_value.data, value_to_package, 127);
    return param_value;
}

uavcan_protocol_param_Value package_uavcan_param_value_empty() {
    uavcan_protocol_param_Value param_value{};
    return param_value;
}

uavcan_protocol_param_NumericValue package_uavcan_param_numeric_value(int32_t value_to_package) {
    uavcan_protocol_param_NumericValue numeric_param_value;
    numeric_param_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE;
    numeric_param_value.integer_value = value_to_package;
    return numeric_param_value;
}

uavcan_protocol_param_NumericValue package_uavcan_param_numeric_value(float value_to_package) {
    uavcan_protocol_param_NumericValue numeric_param_value;
    numeric_param_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE;
    numeric_param_value.real_value = value_to_package;
    return numeric_param_value;
}

uavcan_protocol_param_NumericValue package_uavcan_param_numeric_value_empty() {
    uavcan_protocol_param_NumericValue numeric_param_value;
    numeric_param_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_EMPTY;
    return numeric_param_value;
}

void copy_uavcan_param_value(uavcan_protocol_param_Value& dest, uavcan_protocol_param_Value& src)
{
    dest.union_tag = src.union_tag;

    switch(src.union_tag)
    {
        case UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY:
            dest.empty.dummy = 0;
            break;
        
        case UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE:
            dest.integer_value = src.integer_value;
            break;

        case UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE:
            dest.real_value = src.real_value;
            break;

        case UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE:
            dest.boolean_value = src.boolean_value;
            break;

        case UAVCAN_PROTOCOL_PARAM_VALUE_STRING_VALUE:
            dest.string_value.len = src.string_value.len;
            strncpy((char*)dest.string_value.data, (char*)src.string_value.data,
                    sizeof(src.string_value.data));
            break;

        default:
            dest.empty.dummy = 0;
            break;
    }
}

using numeric_value_t = uavcan_protocol_param_NumericValue;
void copy_numeric_value(numeric_value_t& dest, numeric_value_t& src)
{
    dest.union_tag = src.union_tag;
    
    switch (src.union_tag)
    {
    case UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_EMPTY:
        dest.empty.dummy = 0;
        break;

    case UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE:
        dest.integer_value = src.integer_value;
        break;

    case UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE:
        dest.real_value = src.real_value;
        break;

    default:
        dest.empty.dummy = 0;
        break;
    }
}