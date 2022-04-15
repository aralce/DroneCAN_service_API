#include "common_to_all_tests.h"
#include <uavcan.equipment.power.BatteryInfo.h>

static DroneCAN_service get_droneCAN_instance_omiting_mock_calls()
{
    mock().disable();
    DroneCAN_service droneCAN_service;
    mock().enable();
    return droneCAN_service;
}

TEST_GROUP(DroneCAN_service_publish_batteryInfo)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    
    void teardown()
    {
        teardown_mocks();
    }
};

TEST(DroneCAN_service_publish_batteryInfo, canardBroadcast_is_called_with_right_ID_and_signature)
{
    mock().expectOneCall("canardBroadcast")
          .withParameter("data_type_signature", UAVCAN_EQUIPMENT_POWER_BATTERYINFO_SIGNATURE)
          .withParameter("data_type_id", UAVCAN_EQUIPMENT_POWER_BATTERYINFO_ID)
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();

    uavcan_equipment_power_BatteryInfo battery_message;
    droneCAN_service.publish_message(battery_message);
}

TEST(DroneCAN_service_publish_batteryInfo, canardBroadcast_is_called_with_right_priority)
{
    mock().expectOneCall("canardBroadcast")
          .withParameter("priority", CANARD_TRANSFER_PRIORITY_LOW)
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();

    uavcan_equipment_power_BatteryInfo battery_message;
    droneCAN_service.publish_message(battery_message);
}


TEST(DroneCAN_service_publish_batteryInfo, encode_function_is_called_with_battery_message)
{
    uavcan_equipment_power_BatteryInfo battery_message;
    mock().expectOneCall("uavcan_equipment_power_BatteryInfo_encode")
          .withParameter("msg", (void*)&battery_message)
          .ignoreOtherParameters();          
    mock().ignoreOtherCalls();

    droneCAN_service.publish_message(battery_message);
}

//- when batteryInfo is published, the canardBroadcast is called with the message and message_length from encodeMessage
// TEST(DroneCAN_service_publish_batteryInfo, canardBroascast_is_called_with_encoded_message)
// {
//     uavcan_equipment_power_BatteryInfo battery_message;
//     const uint32_t ENCODED_MESSAGE_LENGTH = 3; 
//     uint8_t message_buffer[] = {1, 2, 3};
//     mock().expectOneCall("uavcan_equipment_power_BatteryInfo_encode")
//           .withOutputParameterReturning("buffer", &message_buffer, sizeof(message_buffer))
//           .ignoreOtherParameters()
//           .andReturnValue(ENCODED_MESSAGE_LENGTH);
    
//     mock().expectOneCall("canardBroadcast")
//           .withParameter("payload", message_buffer)
//           .withParameter("payload_len", ENCODED_MESSAGE_LENGTH);

//     droneCAN_service.publish_message(battery_message);
// }