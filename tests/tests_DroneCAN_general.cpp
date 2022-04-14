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
    DroneCAN_service service;
    
    CHECK_TRUE(service.is_healthy());
}