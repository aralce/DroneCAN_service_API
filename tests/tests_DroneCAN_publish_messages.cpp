#include "common_to_all_tests.h"
#include <uavcan.equipment.power.BatteryInfo.h>

TEST_GROUP(DroneCAN_service_publish_batteryInfo)
{
    void teardown()
    {
        teardown_mocks();
    }
};

static DroneCAN_service get_droneCAN_instance_omiting_mock_calls()
{
    mock().disable();
    DroneCAN_service service;
    mock().enable();
    return service;
}

TEST(DroneCAN_service_publish_batteryInfo, canardBroadcast_is_called_with_right_ID_and_signature)
{
    DroneCAN_service service = get_droneCAN_instance_omiting_mock_calls();

    mock().expectOneCall("canardBroadcast")
          .withParameter("data_type_signature", UAVCAN_EQUIPMENT_POWER_BATTERYINFO_SIGNATURE)
          .withParameter("data_type_id", UAVCAN_EQUIPMENT_POWER_BATTERYINFO_ID)
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();

    uavcan_equipment_power_BatteryInfo battery_message;
    service.publish_message(battery_message);
}

TEST(DroneCAN_service_publish_batteryInfo, canardBroadcast_is_called_with_right_priority)
{
    DroneCAN_service service = get_droneCAN_instance_omiting_mock_calls();

    mock().expectOneCall("canardBroadcast")
          .withParameter("priority", CANARD_TRANSFER_PRIORITY_LOW)
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();

    uavcan_equipment_power_BatteryInfo battery_message;
    service.publish_message(battery_message);
}


TEST(DroneCAN_service_publish_batteryInfo, encode_function_is_called_with_battery_message)
{
    DroneCAN_service service = get_droneCAN_instance_omiting_mock_calls();
    
    uavcan_equipment_power_BatteryInfo battery_message;
    mock().expectOneCall("uavcan_equipment_power_BatteryInfo_encode")
          .withParameter("msg", (void*)&battery_message)
          .ignoreOtherParameters();          
    mock().ignoreOtherCalls();

    service.publish_message(battery_message);
}

//- when batteryInfo is published, the canardBroadcast is called with the message and message_length from encodeMessage
TEST(DroneCAN_service_publish_batteryInfo, canardBroascast_is_called_with_encoded_message)
{
    FAIL("Continue from here. Next step: tests for CAN bus write sequence.");
}