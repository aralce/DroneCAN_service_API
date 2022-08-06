#include <common_to_all_tests.h>
#include <auxiliary_functions.h>
#include "../common_to_publish_message/common_to_publish_message.h"
#include "common_to_paramGetSet/common_to_paramGetSet.h"

TEST_GROUP(DroneCAN_service_paramGetSet_publish)
{
    void teardown()
    {
        teardown_paramGetSet_tests();
    }
};

TEST(DroneCAN_service_paramGetSet_publish, respond_with_parameter_data)
{
    mock().ignoreOtherCalls();
    Spied_droneCAN_service spied_droneCAN_service;

    uavcan_parameter param_to_send;
    strcpy((char *)param_to_send.name.data, "param_to_send");
    param_to_send.name.len = strlen((char *)param_to_send.name.data);
    const int PARAM_VALUE = 20;
    param_to_send.value = package_uavcan_param_value(PARAM_VALUE);
    
    const uint8_t INDEX_OF_PARAMETER_TO_SEND = 0;
    spied_droneCAN_service.respond_with_parameter_data(INDEX_OF_PARAMETER_TO_SEND);

    mock().expectOneCall("canard->")
}