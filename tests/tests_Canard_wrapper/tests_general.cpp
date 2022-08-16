#include <common_to_all_tests.h>
#include <Canard_wrapper.h>
#include <DroneCAN_service_configuration.h>


TEST_GROUP(Canard_wrapper)
{
    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    }
};

Canard get_canard_instance()
{
    Canard canard(LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, UAVCAN_MAX_BYTES_ON_MESSAGE);
    return canard;
}

TEST(Canard_wrapper, init)
{
    Canard canard = get_canard_instance();
    mock().expectOneCall("canardInit");

    canard.init();
}

TEST(Canard_wrapper, set_node_ID)
{
    Canard canard = get_canard_instance();
    const uint8_t CANARD_NODE_ID = 40;
    mock().expectOneCall("canardSetLocalNodeID")
          .withUnsignedIntParameter("self_node_id", CANARD_NODE_ID);
    
    canard.set_node_ID(CANARD_NODE_ID);
}

    canard_message_type_info_t message_type_info = {.signature = 4,
                                                    .id = 99,
                                                    .priority = CANARD_TRANSFER_PRIORITY_MEDIUM};
    uint8_t data[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    canard_message_data_t message_data = {.ptr = data, .length = 8};
    const uint16_t RETURN_VALUE = 16;
TEST(Canard_wrapper, broadCast)
{
    Canard canard = get_canard_instance();
    
    mock().expectOneCall("canardBroadcast")
          .withUnsignedLongLongIntParameter("data_type_signature", message_type_info.signature)
          .withUnsignedIntParameter("data_type_id", message_type_info.id)
          .withUnsignedIntParameter("priority", message_type_info.priority)
          .withPointerParameter("payload", message_data.ptr)
          .withUnsignedIntParameter("payload_len", message_data.length)
          .andReturnValue(RETURN_VALUE);

    CHECK_EQUAL(RETURN_VALUE, canard.broadcast(message_type_info, message_data));
}

TEST(Canard_wrapper, send_response)
{
    Canard canard = get_canard_instance();

    uint8_t DESTINATION_NODE_ID = 40;
    mock().expectOneCall("canardRequestOrRespond")
          .withUnsignedIntParameter("destination_node_id", DESTINATION_NODE_ID)
          .withUnsignedLongLongIntParameter("data_type_signature", message_type_info.signature)
          .withUnsignedIntParameter("data_type_id", message_type_info.id)
          .withUnsignedIntParameter("priority", message_type_info.priority)
          .withPointerParameter("payload", (void*)message_data.ptr)
          .withUnsignedIntParameter("payload_len", message_data.length)
          .andReturnValue(RETURN_VALUE);
    
    canard.send_response(DESTINATION_NODE_ID, message_type_info, message_data);
}

TEST(Canard_wrapper, peekTxQueue)
{
    Canard canard = get_canard_instance();

    CanardCANFrame can_frame{};
    mock().expectOneCall("canardPeekTxQueue")
          .andReturnValue((void*)&can_frame);

    CHECK_EQUAL(&can_frame, canard.peekTxQueue());
}

TEST(Canard_wrapper, popTxQueue)
{
    Canard canard = get_canard_instance();

    mock().expectOneCall("canardPopTxQueue");
    canard.popTxQueue();
}

TEST(Canard_wrapper, is_txQueue_empty_return_the_queue_has_data)
{
    Canard canard = get_canard_instance();

    CanardCANFrame can_frame{};
    mock().expectOneCall("canardPeekTxQueue")
          .andReturnValue((void*)&can_frame);
    
    CHECK_FALSE(canard.is_txQueue_empty());
}

TEST(Canard_wrapper, is_txQueue_empty_returns_the_queue_is_empty)
{
    Canard canard = get_canard_instance();

    mock().expectOneCall("canardPeekTxQueue")
          .andReturnValue((void*)NULL);

    CHECK_TRUE(canard.is_txQueue_empty());
}

TEST(Canard_wrapper, handle_rx_frame)
{
    Canard canard = get_canard_instance();

    CanardCANFrame canard_frame{};
    uint64_t TIMESTAMP_MICROSECONDS = 10000;
    int16_t ERROR_CODE = -99;
    mock().expectOneCall("canardHandleRxFrame")
          .withPointerParameter("frame", (void*)&canard_frame)
          .withUnsignedLongLongIntParameter("timestamp_usec", TIMESTAMP_MICROSECONDS)
          .andReturnValue(ERROR_CODE);

    int16_t returned_value = canard.handle_rx_frame(canard_frame, TIMESTAMP_MICROSECONDS);
    CHECK_EQUAL(ERROR_CODE, returned_value);
}