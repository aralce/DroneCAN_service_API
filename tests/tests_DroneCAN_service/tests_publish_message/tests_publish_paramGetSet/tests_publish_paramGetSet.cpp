#include <common_to_all_tests.h>
#include "../common_to_publish_message/common_to_publish_message.h"
#include <uavcan.protocol.param.GetSet.h>
#include <auxiliary_functions.h>
#include <cstring>

DroneCAN_service* droneCAN_service = nullptr;

TEST_GROUP(DroneCAN_service_publish_paramGetSet)
{
    void setup()
    {
        mock().ignoreOtherCalls();
        droneCAN_service = new DroneCAN_service;
    }
    void teardown()
    {
        if (droneCAN_service != nullptr) {
            delete droneCAN_service;
            droneCAN_service = nullptr;
        }
        teardown_mocks();
    }
};

TEST(DroneCAN_service_publish_paramGetSet, inits_without_parameters)
{
    CHECK_EQUAL(0, droneCAN_service->get_number_of_parameters());
}

void remove_first_parameter();

TEST(DroneCAN_service_publish_paramGetSet, try_to_remove_a_parameter_when_number_of_parameters_is_zero_has_no_effect)
{
    remove_first_parameter();
    CHECK_EQUAL(0, droneCAN_service->get_number_of_parameters());
}

void add_generic_parameter();

TEST(DroneCAN_service_publish_paramGetSet, when_a_parameter_is_added_then_the_number_of_parameters_is_increased)
{
    add_generic_parameter();
    CHECK_EQUAL(1, droneCAN_service->get_number_of_parameters());
}

TEST(DroneCAN_service_publish_paramGetSet, add_1_parameter_and_remove_1_parameter_then_number_of_parameters_is_zero)
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

TEST(DroneCAN_service_publish_paramGetSet, add_more_parameters_than_the_MAX_NUMBER_OF_PARAMETERS_has_no_effect)
{
    for (uint8_t parameter = 0; parameter < MAX_NUMBER_OF_PARAMETERS + 1; ++parameter)
        add_generic_parameter();
    CHECK_EQUAL(MAX_NUMBER_OF_PARAMETERS, droneCAN_service->get_number_of_parameters());
}

void add_generic_parameter() {
    uavcan_parameter parameter_to_add;
    strcpy(parameter_to_add.name.data, "parameter_to_add");
    parameter_to_add.name.len = strlen(parameter_to_add.name.data);
    
    droneCAN_service->add_parameter(parameter_to_add);
}

TEST(DroneCAN_service_publish_paramGetSet, get_parameter)
{
    uavcan_parameter parameter_to_add;
    const char PARAMETER_NAME[] = "parameter_to_add";
    strcpy(parameter_to_add.name.data, PARAMETER_NAME);
    parameter_to_add.name.len = strlen(parameter_to_add.name.data);
    
    const uint8_t PARAMETER_VALUE = 10;
    parameter_to_add.value = package_uavcan_param_value(PARAMETER_VALUE);
    
    const uint8_t PARAMETER_DEFAULT_VALUE = 50;
    parameter_to_add.default_value = package_uavcan_param_value(PARAMETER_DEFAULT_VALUE);

    const uint8_t PARAMETER_MAX_VALUE = 255;
    parameter_to_add.max_value = package_uavcan_param_value(PARAMETER_MAX_VALUE);

    const uint8_t PARAMETER_MIN_VALUE = 5;
    parameter_to_add.min_value = package_uavcan_param_value(PARAMETER_MIN_VALUE);

    droneCAN_service->add_parameter(parameter_to_add);
    uavcan_parameter parameter_returned = droneCAN_service->get_parameter(0);

    STRCMP_EQUAL(PARAMETER_NAME, parameter_returned.name.data);
    CHECK_EQUAL(PARAMETER_VALUE, parameter_returned.value.integer_value);
    CHECK_EQUAL(PARAMETER_DEFAULT_VALUE, parameter_returned.default_value.integer_value);
    CHECK_EQUAL(PARAMETER_MAX_VALUE, parameter_returned.max_value.integer_value);
    CHECK_EQUAL(PARAMETER_MIN_VALUE, parameter_returned.min_value.integer_value);
}