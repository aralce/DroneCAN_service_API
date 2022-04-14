#include "common_to_all_tests.h"
#include <dronecan_msgs.h>

TEST_GROUP(DroneCAN_service)
{
    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
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

TEST(DroneCAN_service, batteryInfo_message_is_published_then_canardBroadcast_is_called) {
    mock().disable();
    DroneCAN_service service;
    mock().enable();
    
    uavcan_equipment_power_BatteryInfo battery_message;
    mock().expectOneCall("canardBroadcast")
          .ignoreOtherParameters();
    service.publish_message(battery_message);
}