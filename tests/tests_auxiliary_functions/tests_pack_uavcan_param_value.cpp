#include <common_to_all_tests.h>
#include <auxiliary_functions.h>
#include <uavcan.protocol.param.Value.h>

TEST_GROUP(Auxiliary_functions_package_parameter_value)
{

};

template <typename VALUE_TO_PACKAGE>
void check_package_value_integer(VALUE_TO_PACKAGE value);

TEST(Auxiliary_functions_package_parameter_value, package_uint8_t)
{
    uint8_t VALUE_TO_PACKAGE = 20;
    check_package_value_integer(VALUE_TO_PACKAGE);
}

TEST(Auxiliary_functions_package_parameter_value, package_uint16_t)
{
    uint16_t VALUE_TO_PACKAGE = 65000;
    check_package_value_integer(VALUE_TO_PACKAGE);
}

TEST(Auxiliary_functions_package_parameter_value, package_int32_t)
{
    int32_t VALUE_TO_PACKAGE = -1e6;
    check_package_value_integer(VALUE_TO_PACKAGE);
}

template <typename VALUE_TO_PACKAGE>
void check_package_value_integer(VALUE_TO_PACKAGE value)
{
    uavcan_protocol_param_Value param_value = package_uavcan_param_value(value);

    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE, param_value.union_tag);
    CHECK_EQUAL(value, param_value.integer_value);
}

TEST(Auxiliary_functions_package_parameter_value, package_empty)
{
    uavcan_protocol_param_Value param_value = package_uavcan_param_value_empty();

    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY, param_value.union_tag);
}

TEST(Auxiliary_functions_package_parameter_value, package_float)
{
    float VALUE_TO_PACKAGE = 1.2345678;
    uavcan_protocol_param_Value param_value = package_uavcan_param_value(VALUE_TO_PACKAGE);

    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE, param_value.union_tag);
    CHECK_EQUAL(VALUE_TO_PACKAGE, param_value.real_value);
}

TEST(Auxiliary_functions_package_parameter_value, package_boolean)
{
    bool VALUE_TO_PACKAGE = true;
    uavcan_protocol_param_Value param_value = package_uavcan_param_value(VALUE_TO_PACKAGE);

    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE, param_value.union_tag);
    CHECK_TRUE(param_value.boolean_value);
}

template <typename STRING_VALUE>
void check_package_value_string(STRING_VALUE value_to_package);

TEST(Auxiliary_functions_package_parameter_value, package_string_with_char_ptr)
{
    char VALUE_TO_PACKAGE[] = "value";
    check_package_value_string(VALUE_TO_PACKAGE);
}

TEST(Auxiliary_functions_package_parameter_value, package_string_with_uint8_t_ptr)
{
    char VALUE_TO_PACKAGE[] = "value";
    check_package_value_string((uint8_t*)VALUE_TO_PACKAGE);
}

template <typename STRING_VALUE>
void check_package_value_string(STRING_VALUE value_to_package) {
    uavcan_protocol_param_Value param_value = package_uavcan_param_value_string(value_to_package);
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_VALUE_STRING_VALUE, param_value.union_tag);
    CHECK_EQUAL(strlen((const char*)value_to_package), param_value.string_value.len);
    STRCMP_EQUAL((char*)value_to_package, (const char*)param_value.string_value.data);
}

#define COUNT_OF(x) sizeof(x)/sizeof(x[0])
TEST(Auxiliary_functions_package_parameter_value, package_string_with_more_than_MAX_ALLOWED_CHARS_cut_the_string_to_MAX_LENGTH) {
    
    char value_to_package[UAVCAN_PARAM_VALUE_MAX_NAME_LENGTH + 10];
    memset(value_to_package, 'a', COUNT_OF(value_to_package));
    
    const int LAST_CHAR_INDEX = UAVCAN_PARAM_VALUE_MAX_NAME_LENGTH;
    value_to_package[LAST_CHAR_INDEX] = '\0';

    uavcan_protocol_param_Value param_value = package_uavcan_param_value_string(value_to_package);

    int count_of_chars = 0;
    while (param_value.string_value.data[count_of_chars] != '\0')
        ++count_of_chars;

    CHECK_EQUAL(UAVCAN_PARAM_VALUE_MAX_NAME_LENGTH - 1, count_of_chars);
}