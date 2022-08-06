#include <common_to_all_tests.h>
#include <auxiliary_functions.h>
#include <uavcan.protocol.param.NumericValue.h>

TEST_GROUP(Auxiliary_functions_package_param_numeric_value)
{

};

template <typename INT_VALUE_TO_PACKAGE>
void check_numeric_int_value_is_packaged(INT_VALUE_TO_PACKAGE value_to_package)
{
    uavcan_protocol_param_NumericValue param_numeric = package_uavcan_param_numeric_value(value_to_package);
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE, param_numeric.union_tag);
    CHECK_EQUAL(value_to_package, param_numeric.integer_value);
}

TEST(Auxiliary_functions_package_param_numeric_value, package_uint8_t)
{
    const uint8_t VALUE_TO_PACKAGE = 50;
    check_numeric_int_value_is_packaged(VALUE_TO_PACKAGE);
}

TEST(Auxiliary_functions_package_param_numeric_value, package_uint16_t)
{
    const uint16_t VALUE_TO_PACKAGE = 65000;
    check_numeric_int_value_is_packaged(VALUE_TO_PACKAGE);
}

TEST(Auxiliary_functions_package_param_numeric_value, package_int32_t)
{
    const int32_t VALUE_TO_PACKAGE = -2e6;
    check_numeric_int_value_is_packaged(VALUE_TO_PACKAGE);
}

// enum uavcan_protocol_param_NumericValue_type_t {
//     UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_EMPTY,
//     UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE,
//     UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE,
// };

// struct uavcan_protocol_param_NumericValue {
//     enum uavcan_protocol_param_NumericValue_type_t union_tag;
//     union {
//         struct uavcan_protocol_param_Empty empty;
//         int64_t integer_value;
//         float real_value;
//     };
// };