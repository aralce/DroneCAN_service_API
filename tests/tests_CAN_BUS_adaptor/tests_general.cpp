#include <common_to_all_tests.h>
#include <CAN_bus_adaptor.h>
#include <cstring>

#define COUNT_OF(x) sizeof(x)/sizeof(x[0])

TEST_GROUP(CAN_bus_adaptor)
{
    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(CAN_bus_adaptor, begin)
{
    CAN_bus_adaptor can;

    const long BAUDRATE = 1e6;
    mock().expectOneCall("can_class->begin")
          .withLongIntParameter("baudRate", BAUDRATE);

    can.begin(BAUDRATE);
}

TEST(CAN_bus_adaptor, setPins)
{
    CAN_bus_adaptor can;

    const int RX_PIN = 0;
    const int TX_PIN = 1;
    mock().expectOneCall("can_class->setPins")
          .withIntParameter("rx", RX_PIN)
          .withIntParameter("tx", TX_PIN);

    can.setPins(RX_PIN, TX_PIN);
}

TEST(CAN_bus_adaptor, send_can_frame_with_success)
{
    CAN_bus_adaptor can;

    CanardCANFrame can_frame;
    can_frame.id = 32;
    uint8_t frame_data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    memcpy(can_frame.data, frame_data, 8);
    can_frame.data_len = COUNT_OF(frame_data);

    mock().expectOneCall("can_class->beginExtendedPacket")
          .withLongIntParameter("id", can_frame.id & CANARD_CAN_EXT_ID_MASK)
          .andReturnValue(true);
    
    mock().expectOneCall("can_class->write")
          .withPointerParameter("buffer", can_frame.data)
          .withLongIntParameter("size", can_frame.data_len)
          .andReturnValue(can_frame.data_len);
    
    mock().expectOneCall("can_class->endPacket")
          .andReturnValue(true);

    CHECK_TRUE(can.send_frame(can_frame));
}

TEST(CAN_bus_adaptor, send_can_frame_fails_on_beginExtendedPacket)
{
    CAN_bus_adaptor can;

    mock().expectOneCall("can_class->beginExtendedPacket")
          .ignoreOtherParameters()
          .andReturnValue(false);
    mock().ignoreOtherCalls();

    CanardCANFrame can_frame;
    CHECK_FALSE(can.send_frame(can_frame));
}

TEST(CAN_bus_adaptor, send_can_frame_fails_on_write_function)
{
    CAN_bus_adaptor can;

    const size_t CAN_FRAME_SIZE = 8;
    const size_t VALUE_DIFFERENT_FROM_CAN_FRAME_SIZE = CAN_FRAME_SIZE - 1;
    mock().expectOneCall("can_class->write")
          .withUnsignedLongIntParameter("size", CAN_FRAME_SIZE)
          .ignoreOtherParameters()
          .andReturnValue(VALUE_DIFFERENT_FROM_CAN_FRAME_SIZE);
    mock().ignoreOtherCalls();

    CanardCANFrame can_frame;
    can_frame.data_len = CAN_FRAME_SIZE;
    CHECK_FALSE(can.send_frame(can_frame));
}