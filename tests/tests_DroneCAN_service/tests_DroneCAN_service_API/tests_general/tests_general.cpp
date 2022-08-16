#include <common_to_DroneCAN_service_tests.h>
#include <DroneCAN_message_sender.h>

TEST_GROUP(DroneCAN_service_API_general)
{
    CanardCANFrame_comparator can_frame_comparator;
    void setup()
    {
        mock().installComparator("CanardCANFrame", can_frame_comparator);
    }
    void teardown()
    {
        mock().removeAllComparatorsAndCopiers();
        mock().checkExpectations();
        mock().clear();
    }
};

extern void onReceive_on_can_bus(int packet_size);
extern bool is_can_data_to_read;
TEST(DroneCAN_service_API_general, onReceive_from_can_bus_read_data_and_sends_it_to_canard)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    
    const int CAN_BUS_PACKET_SIZE = 8;
    uint8_t can_bus_packet[CAN_BUS_PACKET_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};
    
    is_can_data_to_read = true;

    const long CAN_BUS_PACKET_ID = 99;
    mock().expectOneCall("CAN_bus_adaptor->get_packet_id")
          .andReturnValue(CAN_BUS_PACKET_ID);

    for (int byte = 0; byte < CAN_BUS_PACKET_SIZE; ++byte) {
        mock().expectOneCall("CAN_bus_adaptor->read")
              .andReturnValue(can_bus_packet[byte]);
    }

    milliseconds ACTUAL_TIME = 100;
    CanardCANFrame can_frame{};
    can_frame.id = CAN_BUS_PACKET_ID;
    memcpy(can_frame.data, can_bus_packet, CAN_BUS_PACKET_SIZE);
    can_frame.data_len = CAN_BUS_PACKET_SIZE;
    mock().expectOneCall("canard->handle_rx_frame")
          .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&can_frame)
          .withUnsignedLongLongIntParameter("timestamp_usec", 1000*ACTUAL_TIME);

    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

static void handle_error(DroneCAN_error error) {
    mock().actualCall("canard->handle_error")
          .withIntParameter("error", (int)error);
}

TEST(DroneCAN_service_API_general, when_onReceive_from_can_bus_has_error_then_indicates_it)
{
    mock().disable();
    DroneCAN_service droneCAN_service(DEFAULT_NODE_ID, handle_error);
    mock().enable();

    is_can_data_to_read = true;
    int16_t ERROR_CODE = -40;
    mock().expectOneCall("canard->handle_rx_frame")
          .ignoreOtherParameters()
          .andReturnValue(ERROR_CODE);
    mock().expectOneCall("canard->handle_error")
          .withIntParameter("error", (int)DroneCAN_error::FAIL_ON_RECEPTION);
    mock().ignoreOtherCalls();
    
    milliseconds ACTUAL_TIME = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}