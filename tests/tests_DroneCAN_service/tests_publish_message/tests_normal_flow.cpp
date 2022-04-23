#include <uavcan.equipment.power.BatteryInfo.h>
#include "common_to_publish_message/common_to_publish_message.h"

TEST_GROUP(DroneCAN_service_publish)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    Canard_message_type_info_t_comparator type_info_comparator;
    Canard_message_data_t_comparator data_comparator;
    CanardCANFrame_comparator can_frame_comparator;
    
    void setup()
    {
        mock().installComparator("canard_message_type_info_t", type_info_comparator);
        mock().installComparator("canard_message_data_t", data_comparator);
        mock().installComparator("CanardCANFrame", can_frame_comparator);
    }
    void teardown()
    {
        execute_test_teardown();
    }
};


TEST(DroneCAN_service_publish, encode_function_is_called_with_battery_message)
{
    uavcan_equipment_power_BatteryInfo battery_message;
    mock().expectOneCall("uavcan_equipment_power_BatteryInfo_encode")
          .withParameter("msg", (void*)&battery_message)
          .ignoreOtherParameters();          
    
    mock().ignoreOtherCalls();
    droneCAN_service.publish_message(battery_message);
}


TEST(DroneCAN_service_publish, canardBroadcast_is_called_with_right_type_info)
{
    canard_message_type_info_t type_info = {.signature = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_SIGNATURE,
                                            .id = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_ID,
                                            .priority = CANARD_TRANSFER_PRIORITY_LOW };
    mock().expectOneCall("broadcast")
          .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&type_info)
          .ignoreOtherParameters();

    mock().ignoreOtherCalls();
    publish_batteryInfo_message(droneCAN_service); 
}


TEST(DroneCAN_service_publish, canardBroadcast_is_called_with_right_data)
{
    const uint32_t ENCODED_MESSAGE_LENGTH = 3;
    uint8_t message_buffer[ENCODED_MESSAGE_LENGTH] = {12,13,14};   
    mock().expectOneCall("uavcan_equipment_power_BatteryInfo_encode")
          .withOutputParameterReturning("buffer", message_buffer, ENCODED_MESSAGE_LENGTH)
          .ignoreOtherParameters()
          .andReturnValue(ENCODED_MESSAGE_LENGTH);

    canard_message_data_t data = {.ptr = (void*)message_buffer,
                                  .length = ENCODED_MESSAGE_LENGTH };
    mock().expectOneCall("broadcast")
          .withParameterOfType("canard_message_data_t", "data", (const void*)&data)
          .ignoreOtherParameters();
    
    mock().ignoreOtherCalls(); 
    publish_batteryInfo_message(droneCAN_service);
}


void extract_and_send_all_CAN_frames();

TEST(DroneCAN_service_publish, canard_extract_all_pending_frames_and_send_with_CAN_bus_driver)
{
    extract_and_send_all_CAN_frames();

    mock().expectOneCall("is_txQueue_empty")
          .andReturnValue((int)true);
    
    mock().ignoreOtherCalls();

    publish_batteryInfo_message(droneCAN_service);
}

void extract_and_send_all_CAN_frames()
{
    CanardCANFrame returned_frame{.id = 0x42, 
                                  .data = {1,2,3,4,5,6,7,8},
                                  .data_len = 8};
    
    const int16_t CAN_FRAMES_TO_SEND_ON_PUBLISH = 10;
    mock().expectNCalls(CAN_FRAMES_TO_SEND_ON_PUBLISH, "peekTxQueue")
          .andReturnValue((uint64_t)&returned_frame);  

    mock().expectNCalls(CAN_FRAMES_TO_SEND_ON_PUBLISH, "is_txQueue_empty")
          .andReturnValue((int)false);

    mock().expectNCalls(CAN_FRAMES_TO_SEND_ON_PUBLISH, "send_frame")
          .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&returned_frame);
}


TEST(DroneCAN_service_publish, success_on_send_CAN_message)
{
    mock().expectOneCall("is_txQueue_empty")
          .andReturnValue((int)false);

    const int SUCCESS_ON_SEND = 1;
    mock().expectOneCall("send_frame")
          .ignoreOtherParameters()
          .andReturnValue(SUCCESS_ON_SEND);

    mock().expectOneCall("is_txQueue_empty")
          .andReturnValue((int)true);

    mock().ignoreOtherCalls();

    publish_batteryInfo_message(droneCAN_service);
}