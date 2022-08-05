#include <common_to_all_tests.h>
#include "../common_to_publish_message/common_to_publish_message.h"
#include <uavcan.protocol.param.GetSet.h>
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
    strcpy(parameter_to_add.name.data, "parameter_to_add");
    parameter_to_add.name.len = strlen(parameter_to_add.name.data);
    parameter_to_add.value = package_uavcan_parameter((uint8_t)10);
}

struct uavcan_protocol_param_GetSetResponse {
    struct uavcan_protocol_param_Value value;
    struct uavcan_protocol_param_Value default_value;
    struct uavcan_protocol_param_NumericValue max_value;
    struct uavcan_protocol_param_NumericValue min_value;
    struct { uint8_t len; uint8_t data[92]; }name;
};