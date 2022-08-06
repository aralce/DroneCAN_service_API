#include <common_to_all_tests.h>
#include <auxiliary_functions.h>
#include <uavcan.protocol.param.NumericValue.h>

TEST_GROUP(Auxiliary_functions_package_param_numeric_value)
{

};

template <typename INT_VALUE_TO_PACKAGE>
void check_numeric_int_value_is_packaged(INT_VALUE_TO_PACKAGE value_to_package);

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

template <typename INT_VALUE_TO_PACKAGE>
void check_numeric_int_value_is_packaged(INT_VALUE_TO_PACKAGE value_to_package)
{
    uavcan_protocol_param_NumericValue param_numeric = package_uavcan_param_numeric_value(value_to_package);
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE, param_numeric.union_tag);
    CHECK_EQUAL(value_to_package, param_numeric.integer_value);
}

TEST(Auxiliary_functions_package_param_numeric_value, package_float)
{
    const float VALUE_TO_PACKAGE = 1.23456789;
    uavcan_protocol_param_NumericValue param_numeric = package_uavcan_param_numeric_value(VALUE_TO_PACKAGE);
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE, param_numeric.union_tag);
    CHECK_EQUAL(VALUE_TO_PACKAGE, param_numeric.real_value);
}

TEST(Auxiliary_functions_package_param_numeric_value, package_empty_value)
{
    uavcan_protocol_param_NumericValue param_numeric = package_uavcan_param_numeric_value_empty();
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_EMPTY, param_numeric.union_tag);
}