#include <common_to_all_tests.h>
#include <common_to_DroneCAN_service_tests.h>
#include <uavcan.protocol.param.GetSet.h>
#include <auxiliary_functions.h>
#include <cstring>

DroneCAN_service* droneCAN_service = nullptr;

TEST_GROUP(DroneCAN_service_paramGetSet_parameters)
{
    void setup()
    {
        mock().ignoreOtherCalls();
        droneCAN_service = new DroneCAN_service;
    }
    void teardown()
    {
        teardown_mocks();
        if (droneCAN_service != nullptr) {
            delete droneCAN_service;
            droneCAN_service = nullptr;
        }
    }
};

TEST(DroneCAN_service_paramGetSet_parameters, inits_without_parameters)
{
    CHECK_EQUAL(0, droneCAN_service->get_number_of_parameters());
}

void remove_first_parameter();

TEST(DroneCAN_service_paramGetSet_parameters, try_to_remove_a_parameter_when_number_of_parameters_is_zero_has_no_effect)
{
    remove_first_parameter();
    CHECK_EQUAL(0, droneCAN_service->get_number_of_parameters());
}

void add_generic_parameter(uavcan_protocol_param_Value_type_t value_type = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE);

TEST(DroneCAN_service_paramGetSet_parameters, when_a_parameter_is_added_then_the_number_of_parameters_is_increased)
{
    add_generic_parameter();
    CHECK_EQUAL(1, droneCAN_service->get_number_of_parameters());
}

TEST(DroneCAN_service_paramGetSet_parameters, add_1_parameter_and_remove_1_parameter_then_number_of_parameters_is_zero)
{
    add_generic_parameter();
    remove_first_parameter();
    CHECK_EQUAL(0, droneCAN_service->get_number_of_parameters());
}

void remove_first_parameter()
{
    const uint8_t PARAMETER_INDEX_TO_REMOVE = 0;
    droneCAN_service->remove_parameter(PARAMETER_INDEX_TO_REMOVE);
}

TEST(DroneCAN_service_paramGetSet_parameters, add_more_parameters_than_the_MAX_NUMBER_OF_PARAMETERS_has_no_effect)
{
    for (uint8_t parameter = 0; parameter < MAX_NUMBER_OF_PARAMETERS + 1; ++parameter)
        add_generic_parameter();
    CHECK_EQUAL(MAX_NUMBER_OF_PARAMETERS, droneCAN_service->get_number_of_parameters());
}

TEST(DroneCAN_service_paramGetSet_parameters, get_parameter)
{
    uavcan_parameter parameter_to_add;
    const char PARAMETER_NAME[] = "parameter_to_add";
    strcpy((char*)parameter_to_add.name.data, PARAMETER_NAME);
    parameter_to_add.name.len = strlen((const char*)parameter_to_add.name.data);
    
    const uint8_t PARAMETER_VALUE = 10;
    parameter_to_add.value = package_uavcan_param_value(PARAMETER_VALUE);
    
    const uint8_t PARAMETER_DEFAULT_VALUE = 50;
    parameter_to_add.default_value = package_uavcan_param_value(PARAMETER_DEFAULT_VALUE);

    const uint8_t PARAMETER_MAX_VALUE = 255;
    parameter_to_add.max_value = package_uavcan_param_numeric_value(PARAMETER_MAX_VALUE);

    const uint8_t PARAMETER_MIN_VALUE = 5;
    parameter_to_add.min_value = package_uavcan_param_numeric_value(PARAMETER_MIN_VALUE);

    droneCAN_service->add_parameter(parameter_to_add);
    uavcan_parameter parameter_returned = droneCAN_service->get_parameter(0);

    STRCMP_EQUAL(PARAMETER_NAME, (const char*)parameter_returned.name.data);
    CHECK_EQUAL(PARAMETER_VALUE, parameter_returned.value.integer_value);
    CHECK_EQUAL(PARAMETER_DEFAULT_VALUE, parameter_returned.default_value.integer_value);
    CHECK_EQUAL(PARAMETER_MAX_VALUE, parameter_returned.max_value.integer_value);
    CHECK_EQUAL(PARAMETER_MIN_VALUE, parameter_returned.min_value.integer_value);
}

TEST(DroneCAN_service_paramGetSet_parameters, get_parameter_when_droneCAN_service_has_no_parameter_returns_a_parameter_with_name_INVALID)
{
    uavcan_parameter parameter_returned = droneCAN_service->get_parameter(0);
    STRCMP_EQUAL(NAME_FOR_INVALID_PARAMETER, (const char*)parameter_returned.name.data);
}

TEST(DroneCAN_service_paramGetSet_parameters, get_parameter_outside_bounds_returns_a_parameter_with_name_INVALID)
{
    const int PARAMETERS_ADDED = 4;
    for ( int i = 0; i < PARAMETERS_ADDED; ++i)
        add_generic_parameter();
    
    const int BEYOND_LAST_INDEX = PARAMETERS_ADDED;
    uavcan_parameter parameter_returned = droneCAN_service->get_parameter(BEYOND_LAST_INDEX);
    STRCMP_EQUAL(NAME_FOR_INVALID_PARAMETER, (const char*)parameter_returned.name.data);
}

TEST(DroneCAN_service_paramGetSet_parameters, get_N_parameters)
{
    const int PARAMETERS_ADDED = 1;
    for ( int i = 0; i < PARAMETERS_ADDED; ++i) {
        add_generic_parameter();
        uavcan_parameter parameter_returned = droneCAN_service->get_parameter(0);
        STRCMP_EQUAL("parameter_to_add", (const char*)parameter_returned.name.data);
    }
}

TEST(DroneCAN_service_paramGetSet_parameters, after_add_1_and_remove_1_parameter_get_a_parameter_returns_with_INVALID_name)
{
    add_generic_parameter();
    droneCAN_service->remove_parameter(0);

    uavcan_parameter parameter_returned =  droneCAN_service->get_parameter(0);
    STRCMP_EQUAL(NAME_FOR_INVALID_PARAMETER, (const char*)parameter_returned.name.data);
}

TEST(DroneCAN_service_paramGetSet_parameters, change_value_to_an_invalid_parameter_returns_false)
{
    const uint8_t PARAMETER_INDEX = 1;
    const uint16_t NEW_VALUE = 0;
    CHECK_FALSE(droneCAN_service->set_parameter_value(PARAMETER_INDEX, NEW_VALUE));
}

TEST(DroneCAN_service_paramGetSet_parameters, change_value_to_a_valid_parameter_returns_true)
{
    add_generic_parameter();
    const uint8_t PARAMETER_INDEX = 0;
    const uint16_t NEW_VALUE = 0;
    CHECK_TRUE(droneCAN_service->set_parameter_value(PARAMETER_INDEX, NEW_VALUE));
}

const uint8_t PARAMETER_INDEX = 0;
void set_parameter_for_change_parameter(uavcan_protocol_param_Value_type_t value_type, uint8_t parameter_index);

TEST(DroneCAN_service_paramGetSet_parameters, change_value_to_an_uint16_t_parameter)
{
    set_parameter_for_change_parameter(UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE, PARAMETER_INDEX);

    uint16_t NEW_VALUE = 987;
    droneCAN_service->set_parameter_value(PARAMETER_INDEX, NEW_VALUE);
    uavcan_parameter parameter_changed = droneCAN_service->get_parameter(PARAMETER_INDEX);
    CHECK_EQUAL(NEW_VALUE, parameter_changed.value.integer_value);
}

TEST(DroneCAN_service_paramGetSet_parameters, change_value_to_an_bool_parameter)
{
    set_parameter_for_change_parameter(UAVCAN_PROTOCOL_PARAM_VALUE_BOOLEAN_VALUE, PARAMETER_INDEX);

    bool NEW_VALUE = true;
    droneCAN_service->set_parameter_value(PARAMETER_INDEX, NEW_VALUE);
    uavcan_parameter parameter_changed = droneCAN_service->get_parameter(PARAMETER_INDEX);
    CHECK_EQUAL(NEW_VALUE, parameter_changed.value.boolean_value);
}

TEST(DroneCAN_service_paramGetSet_parameters, change_value_to_an_float_parameter)
{
    set_parameter_for_change_parameter(UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE, PARAMETER_INDEX);

    float NEW_VALUE = true;
    droneCAN_service->set_parameter_value(PARAMETER_INDEX, NEW_VALUE);
    uavcan_parameter parameter_changed = droneCAN_service->get_parameter(PARAMETER_INDEX);
    CHECK_EQUAL(NEW_VALUE, parameter_changed.value.real_value);
}

void set_parameter_for_change_parameter(uavcan_protocol_param_Value_type_t value_type, uint8_t parameter_index)
{
    add_generic_parameter(value_type);
    uavcan_parameter parameter_added = droneCAN_service->get_parameter(parameter_index);
    CHECK_EQUAL(value_type, parameter_added.value.union_tag);
    uint16_t OLD_VALUE = 0;
    CHECK_EQUAL(OLD_VALUE, parameter_added.value.integer_value);
}

void add_generic_parameter(uavcan_protocol_param_Value_type_t value_type)
{
    uavcan_parameter parameter_to_add{};
    strcpy((char*)parameter_to_add.name.data, "parameter_to_add");
    parameter_to_add.name.len = strlen((const char*)parameter_to_add.name.data);
    parameter_to_add.value.union_tag = value_type;
    
    droneCAN_service->add_parameter(parameter_to_add);
}