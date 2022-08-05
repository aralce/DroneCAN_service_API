#include <common_to_all_tests.h>
#include "../common_to_publish_message/common_to_publish_message.h"

TEST_GROUP(DroneCAN_service_publish_paramGetSet)
{
    void teardown()
    {
        teardown_mocks();
    }
};

TEST(DroneCAN_service_publish_paramGetSet, inits_without_parameters)
{
    mock().ignoreOtherCalls();
    DroneCAN_service droneCAN_service;

    CHECK_EQUAL(0, droneCAN_service.get_number_of_parameters());
}

// TEST(DroneCAN_service_publish_paramGetSet, try_to_remove_a_parameter_when_number_of_parameters_is_zero_has_no_effect)
// {
//     mock().ignoreOtherCalls();
//     DroneCAN_service droneCAN_service;

//     const uint8_t PARAMETER_INDEX_TO_REMOVE = 0;
//     droneCAN_service.remove_parameter(PARAMETER_INDEX_TO_REMOVE);

//     CHECK_EQUAL(0, droneCAN_service.get_number_of_parameters());
// }

TEST(DroneCAN_service_publish_paramGetSet, when_a_parameter_is_added_then_the_number_of_parameters_is_increased)
{
    mock().ignoreOtherCalls();
    DroneCAN_service droneCAN_service;
}