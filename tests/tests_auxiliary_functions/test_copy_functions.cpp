#include <common_to_all_tests.h>
#include <auxiliary_functions.h>

static uavcan_protocol_param_Value param_to_copy{};
static uavcan_protocol_param_Value dest_param{};

TEST_GROUP(Auxiliary_copy_value_functions)
{
    void setup()
    {
        memset(&param_to_copy, 0, sizeof(param_to_copy));
        memset(&dest_param, 0, sizeof(dest_param));
    }
};

TEST(Auxiliary_copy_value_functions, copy_value_empty)
{
    param_to_copy.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY;
    param_to_copy.empty.dummy = 15;

    dest_param.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
    dest_param.empty.dummy = 44;

    copy_uavcan_param_value(dest_param, param_to_copy);
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY, dest_param.union_tag);
    CHECK_EQUAL(0, dest_param.empty.dummy);
}

TEST(Auxiliary_copy_value_functions, copy_value_integer)
{
    param_to_copy.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
    param_to_copy.integer_value = -32;

    dest_param.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY;
    dest_param.integer_value = 45;

    copy_uavcan_param_value(dest_param, param_to_copy);
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE, dest_param.union_tag);
    CHECK_EQUAL(param_to_copy.integer_value, dest_param.integer_value);
}

TEST(Auxiliary_copy_value_functions, copy_value_real)
{
    param_to_copy.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;
    param_to_copy.real_value = 57.94;

    dest_param.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY;
    dest_param.real_value = 12.45;

    copy_uavcan_param_value(dest_param, param_to_copy);
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE, dest_param.union_tag);
    DOUBLES_EQUAL(param_to_copy.real_value, dest_param.real_value, 0.0000001);
}

TEST(Auxiliary_copy_value_functions, copy_value_boolean)
{
    param_to_copy.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE;
    param_to_copy.boolean_value = true;

    dest_param.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY;
    dest_param.boolean_value = false;

    copy_uavcan_param_value(dest_param, param_to_copy);
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE, dest_param.union_tag);
    CHECK_EQUAL(param_to_copy.boolean_value, dest_param.boolean_value);
}

TEST(Auxiliary_copy_value_functions, copy_value_string)
{
    param_to_copy.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_STRING_VALUE;
    char string_to_copy[] = "string to copy";
    memcpy(param_to_copy.string_value.data, string_to_copy, sizeof(string_to_copy));
    param_to_copy.string_value.len = strlen(string_to_copy);

    dest_param.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY;
    char remanent_string[] = "string that it was on the destination string.It is longer on purpose";
    memcpy(dest_param.string_value.data, remanent_string, sizeof(remanent_string));
    dest_param.string_value.len = strlen(remanent_string);

    copy_uavcan_param_value(dest_param, param_to_copy);
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_VALUE_STRING_VALUE, dest_param.union_tag);
    CHECK_EQUAL(param_to_copy.string_value.len, dest_param.string_value.len);
    STRCMP_EQUAL((char*)param_to_copy.string_value.data, (char*)dest_param.string_value.data);

    MEMCMP_EQUAL(param_to_copy.string_value.data, dest_param.string_value.data,
                 sizeof(param_to_copy.string_value.data));
}

static uavcan_protocol_param_NumericValue numeric_to_copy{};
static uavcan_protocol_param_NumericValue numeric_dest{};

TEST_GROUP(Auxiliary_copy_numeric_functions)
{
    void setup()
    {
        memset(&numeric_to_copy, 0, sizeof(numeric_to_copy));
        memset(&numeric_dest, 0, sizeof(numeric_dest));
    }
};

TEST(Auxiliary_copy_numeric_functions, copy_numeric_empty)
{
    numeric_to_copy.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_EMPTY;
    numeric_to_copy.empty.dummy = 50;

    numeric_dest.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE;
    numeric_dest.empty.dummy = 12;

    copy_numeric_value(numeric_dest, numeric_to_copy);

    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_EMPTY, numeric_dest.union_tag);
    CHECK_EQUAL(0, numeric_dest.empty.dummy);
}

TEST(Auxiliary_copy_numeric_functions, copy_numeric_integer)
{
    numeric_to_copy.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE;
    numeric_to_copy.integer_value = -86;

    numeric_dest.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_EMPTY;
    numeric_dest.integer_value = 54;

    copy_numeric_value(numeric_dest, numeric_to_copy);
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE, numeric_dest.union_tag);
    CHECK_EQUAL(numeric_to_copy.integer_value, numeric_dest.integer_value);
}

TEST(Auxiliary_copy_numeric_functions, copy_numeric_real)
{
    numeric_to_copy.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE;
    numeric_to_copy.real_value = 1.7983;

    numeric_dest.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_EMPTY;
    numeric_dest.real_value = 85.769;

    copy_numeric_value(numeric_dest, numeric_to_copy);
    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE, numeric_to_copy.union_tag);
    DOUBLES_EQUAL(numeric_to_copy.real_value, numeric_dest.real_value, 0.000001);
}