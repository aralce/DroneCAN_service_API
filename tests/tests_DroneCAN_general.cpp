#include "common_to_all_tests.h"
// #include <dronecan_msgs.h>

TEST_GROUP(DroneCAN_service)
{
    void teardown()
    {
        teardown_mocks();
    }
};

TEST(DroneCAN_service, system_is_healthy) {
    mock().expectOneCall("begin")
          .withParameter("baudRate", CAN_BUS_BAUDRATE)
          .andReturnValue(INITIALIZATION_SUCCESSFUL);
    mock().ignoreOtherCalls();
    Spied_droneCAN_service droneCAN_service;
    
    CHECK_TRUE(droneCAN_service.is_healthy());
}