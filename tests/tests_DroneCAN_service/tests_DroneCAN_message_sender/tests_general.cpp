#include <common_to_all_tests.h>
#include <DroneCAN_message_sender.h>
#include <DSDL_to_canard_DTO.h>

TEST_GROUP(DroneCAN_message_sender)
{
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
        mock().checkExpectations();
        mock().clear();
        mock().removeAllComparatorsAndCopiers();
    }
};

void handle_error_function(DroneCAN_error error)
{
    if (error == DroneCAN_error::FAIL_ON_PUBLISH)
    mock().actualCall("handle_error_function")
          .withIntParameter("error", (int)error);
}

TEST(DroneCAN_message_sender, broadcast_message_has_error_on_canardBroadcast)
{
    DroneCAN_message_sender message_sender(DEFAULT_NODE_ID, handle_error_function);

    const int16_t ERROR_VALUE = -1;
    mock().expectOneCall("canard->broadcast")
          .ignoreOtherParameters()
          .andReturnValue(ERROR_VALUE);
    mock().expectOneCall("handle_error_function")
          .withIntParameter("error", (int)DroneCAN_error::FAIL_ON_PUBLISH);
    mock().ignoreOtherCalls();

    uavcan_equipment_power_BatteryInfo battery_info;
    message_sender.broadcast_message(battery_info);
}

TEST(DroneCAN_message_sender, broadcast_message_has_error_on_send_to_CAN_BUS)
{
    DroneCAN_message_sender message_sender(DEFAULT_NODE_ID, handle_error_function);

    mock().expectOneCall("canard->is_txQueue_empty")
          .andReturnValue(false);
    mock().expectOneCall("CAN_bus_adaptor->send_frame")
          .ignoreOtherParameters()
          .andReturnValue(false);
    mock().expectOneCall("handle_error_function")
          .withIntParameter("error", (int)DroneCAN_error::FAIL_ON_PUBLISH);
    mock().expectOneCall("canard->is_txQueue_empty")
          .andReturnValue(true);
    mock().ignoreOtherCalls();

    uavcan_equipment_power_BatteryInfo battery_info;
    message_sender.broadcast_message(battery_info);
}

TEST(DroneCAN_message_sender, broadcast_message_with_batteryInfo)
{
    DroneCAN_message_sender message_sender(DEFAULT_NODE_ID);

    mock().expectOneCall("DSDL_to_canard_DTO->construct_with_uavcan_power_BatteryInfo");
    
    canard_message_type_info_t type_info{};
    mock().expectOneCall("DSDL_to_canard_DTO->get_type_info")
          .andReturnValue((void*)&type_info);
    
    canard_message_data_t message_data{};
    mock().expectOneCall("DSDL_to_canard_DTO->get_data")
          .andReturnValue((void*)&message_data);

    const uint16_t FRAMES_TO_TRANSFER = 16;
    mock().expectOneCall("canard->broadcast")
          .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&type_info)
          .withParameterOfType("canard_message_data_t", "data", (const void*)&message_data)
          .andReturnValue(FRAMES_TO_TRANSFER);

    for (int frames = 0; frames < FRAMES_TO_TRANSFER; ++frames) {
        mock().expectOneCall("canard->is_txQueue_empty")
              .andReturnValue(false);
        
        CanardCANFrame* can_frame{};
        mock().expectOneCall("canard->peekTxQueue")
              .andReturnValue((void*)&can_frame);
        mock().expectOneCall("canard->popTxQueue");
        
        mock().expectOneCall("CAN_bus_adaptor->send_frame")
              .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&can_frame)
              .andReturnValue(true);
    }
    mock().expectOneCall("canard->is_txQueue_empty")
          .andReturnValue(true);

    uavcan_equipment_power_BatteryInfo battery_info;
    message_sender.broadcast_message(battery_info);
}

// //This TEST is part of the DroneCAN_message_sender && DSDL_to_canard_DTO
// IGNORE_TEST(DroneCAN_service_publish, canardBroadcast_is_called_with_right_type_info)
// {
//     canard_message_type_info_t type_info = {.signature = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_SIGNATURE,
//                                             .id = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_ID,
//                                             .priority = CANARD_TRANSFER_PRIORITY_MEDIUM };
//     mock().expectOneCall("canard->broadcast")
//           .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&type_info)
//           .ignoreOtherParameters();

//     mock().ignoreOtherCalls();
//     publish_batteryInfo_message(droneCAN_service); 
// }