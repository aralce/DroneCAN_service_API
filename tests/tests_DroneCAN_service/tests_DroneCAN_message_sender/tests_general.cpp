#include <common_to_all_tests.h>
#include <DroneCAN_message_sender.h>
#include <DSDL_to_canard_DTO.h>

canard_message_type_info_t type_info{};
canard_message_data_t message_data{};
DroneCAN_message_sender* message_sender = nullptr;

TEST_GROUP(DroneCAN_message_sender)
{
    Canard_message_type_info_t_comparator type_info_comparator;
    Canard_message_data_t_comparator data_comparator;
    CanardCANFrame_comparator can_frame_comparator;
    void setup()
    {
        message_sender = new DroneCAN_message_sender;
        mock().installComparator("canard_message_type_info_t", type_info_comparator);
        mock().installComparator("canard_message_data_t", data_comparator);
        mock().installComparator("CanardCANFrame", can_frame_comparator);
    }
    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
        mock().removeAllComparatorsAndCopiers();
        if  (message_sender != nullptr) {
            delete message_sender;
            message_sender = nullptr;
        }
    }
};

TEST(DroneCAN_message_sender, inits_healthy)
{
    CHECK_TRUE(message_sender->is_healthy());
}

void handle_error_function(DroneCAN_error error);

TEST(DroneCAN_message_sender, broadcast_message_has_error_on_canardBroadcast)
{
    DroneCAN_message_sender message_sender(handle_error_function);

    const int16_t ERROR_VALUE = -1;
    mock().expectOneCall("canard->broadcast")
          .ignoreOtherParameters()
          .andReturnValue(ERROR_VALUE);
    mock().expectOneCall("handle_error_function")
          .withIntParameter("error", (int)DroneCAN_error::FAIL_ON_PUBLISH);
    mock().ignoreOtherCalls();

    uavcan_equipment_power_BatteryInfo battery_info;
    message_sender.broadcast_message(battery_info);

    CHECK_FALSE(message_sender.is_healthy());
}

TEST(DroneCAN_message_sender, send_response_message_has_error_on_canardRequestOrRespond)
{
    DroneCAN_message_sender message_sender(handle_error_function);

    const int16_t ERROR_VALUE = -1;
    mock().expectOneCall("canard->send_response")
          .ignoreOtherParameters()
          .andReturnValue(ERROR_VALUE);
    mock().expectOneCall("handle_error_function")
          .withIntParameter("error", (int)DroneCAN_error::FAIL_ON_PUBLISH);
    mock().ignoreOtherCalls();

    uavcan_protocol_param_GetSetResponse param_response;
    const uint8_t DESTINATION_NODE_ID = 22;
    message_sender.send_response_message(param_response, DESTINATION_NODE_ID);

    CHECK_FALSE(message_sender.is_healthy());
}

TEST(DroneCAN_message_sender, send_to_CAN_BUS)
{
    DroneCAN_message_sender message_sender(handle_error_function);

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
    
    CHECK_FALSE(message_sender.is_healthy());
}

void handle_error_function(DroneCAN_error error)
{
    if (error == DroneCAN_error::FAIL_ON_PUBLISH)
    mock().actualCall("handle_error_function")
          .withIntParameter("error", (int)error);
}

void CHECK_canard_message_is_sent_with_Canard_and_CAN_bus(canard_message_type_info_t& type_info, canard_message_data_t& message_data);
void CHECK_frames_are_sent_with_CAN_bus(int16_t frames_to_transfer);

TEST(DroneCAN_message_sender, broadcast_message_with_uavcan_power_BatteryInfo)
{
    mock().expectOneCall("DSDL_to_canard_DTO->construct_with_uavcan_power_BatteryInfo");
    CHECK_canard_message_is_sent_with_Canard_and_CAN_bus(type_info, message_data);
    
    uavcan_equipment_power_BatteryInfo battery_info;
    message_sender->broadcast_message(battery_info);
}

TEST(DroneCAN_message_sender, broadcast_message_with_uavcan_protocol_NodeStatus)
{
    mock().expectOneCall("DSDL_to_canard_DTO->constuct_with_uavcan_protocol_NodeStatus");
    CHECK_canard_message_is_sent_with_Canard_and_CAN_bus(type_info, message_data);

    uavcan_protocol_NodeStatus node_status;
    message_sender->broadcast_message(node_status);
}

TEST(DroneCAN_message_sender, send_response_of_uavcan_protocol_paramGetSet)
{
    mock().expectOneCall("DSDL_to_canard_DTO->constuct_with_uavcan_protocol_param_GetSetResponse");

    mock().expectOneCall("DSDL_to_canard_DTO->get_type_info")
          .andReturnValue((void*)&type_info);

    mock().expectOneCall("DSDL_to_canard_DTO->get_data")
          .andReturnValue((void*)&message_data);

    const uint16_t FRAMES_TO_TRANSFER = 9;
    const uint8_t DESTINATION_NODE_ID = 22;
    mock().expectOneCall("canard->send_response")
          .withUnsignedIntParameter("destination_node_id", DESTINATION_NODE_ID)
          .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&type_info)
          .withParameterOfType("canard_message_data_t", "data", (const void*)&message_data)
          .andReturnValue(FRAMES_TO_TRANSFER);

    CHECK_frames_are_sent_with_CAN_bus(FRAMES_TO_TRANSFER);

    uavcan_protocol_param_GetSetResponse param_response;
    message_sender->send_response_message(param_response, DESTINATION_NODE_ID);
}

void CHECK_canard_message_is_sent_with_Canard_and_CAN_bus(canard_message_type_info_t& type_info, canard_message_data_t& message_data)
{
    mock().expectOneCall("DSDL_to_canard_DTO->get_type_info")
          .andReturnValue((void*)&type_info);
    
    mock().expectOneCall("DSDL_to_canard_DTO->get_data")
          .andReturnValue((void*)&message_data);

    const uint16_t FRAMES_TO_TRANSFER = 16;
    mock().expectOneCall("canard->broadcast")
          .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&type_info)
          .withParameterOfType("canard_message_data_t", "data", (const void*)&message_data)
          .andReturnValue(FRAMES_TO_TRANSFER);

    CHECK_frames_are_sent_with_CAN_bus(FRAMES_TO_TRANSFER);
}

void CHECK_frames_are_sent_with_CAN_bus(int16_t frames_to_transfer)
{
    for (int frames = 0; frames < frames_to_transfer; ++frames) {
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
}