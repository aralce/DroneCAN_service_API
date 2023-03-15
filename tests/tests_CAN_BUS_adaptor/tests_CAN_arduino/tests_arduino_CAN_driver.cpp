#include <common_to_all_tests.h>
#include <CAN_bus_adaptor/CAN_drivers_implementation/Arduino/Arduino_CAN_driver.h>
#include <cstring>

#define COUNT_OF(x) sizeof(x)/sizeof(x[0])
static CAN_bus_adaptor* can = nullptr;

TEST_GROUP(CAN_arduino_driver)
{
    void setup ()
    {
        can = new Arduino_CAN_driver();
    }
    void teardown()
    {
        delete can;
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(CAN_arduino_driver, begin)
{
    const long BAUDRATE = 1e6;
    mock().expectOneCall("can_class->begin")
          .withLongIntParameter("baudRate", BAUDRATE);

    can->begin(CAN_bitrate::CAN_1MBITS);
}

TEST(CAN_arduino_driver, setPins)
{
    const int RX_PIN = 0;
    const int TX_PIN = 1;
    mock().expectOneCall("can_class->setPins")
          .withIntParameter("rx", RX_PIN)
          .withIntParameter("tx", TX_PIN);

    can->setPins(RX_PIN, TX_PIN);
}

TEST(CAN_arduino_driver, send_can_frame_with_success)
{
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

    CHECK_TRUE(can->send_frame(can_frame));
}

TEST(CAN_arduino_driver, send_can_frame_fails_on_beginExtendedPacket)
{
    mock().expectOneCall("can_class->beginExtendedPacket")
          .ignoreOtherParameters()
          .andReturnValue(false);
    mock().ignoreOtherCalls();

    CanardCANFrame can_frame;
    CHECK_FALSE(can->send_frame(can_frame));
}

TEST(CAN_arduino_driver, send_can_frame_fails_on_write_function)
{
    const size_t CAN_FRAME_SIZE = 8;
    const size_t VALUE_DIFFERENT_FROM_CAN_FRAME_SIZE = CAN_FRAME_SIZE - 1;
    mock().expectOneCall("can_class->write")
          .withUnsignedLongIntParameter("size", CAN_FRAME_SIZE)
          .ignoreOtherParameters()
          .andReturnValue(VALUE_DIFFERENT_FROM_CAN_FRAME_SIZE);
    mock().ignoreOtherCalls();

    CanardCANFrame can_frame;
    can_frame.data_len = CAN_FRAME_SIZE;
    CHECK_FALSE(can->send_frame(can_frame));
}

typedef void (*onReceive_callback)(int packet_size);
void onReceive_callback_dummy(int) {}

TEST(CAN_arduino_driver, register_an_on_receive_callback_function)
{
    onReceive_callback callback = onReceive_callback_dummy;

    mock().expectOneCall("can_class->onReceive")
          .withPointerParameter("onReceive_callback", (void*)callback);
    can->onReceive(callback);
}

TEST(CAN_arduino_driver, read_from_can)
{
    uint8_t packet_bytes[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    
    for (int bytes = 0; bytes < 8; ++bytes) {
        mock().expectOneCall("can_class->read")
              .andReturnValue(packet_bytes[bytes]);
        can->read();
    }

    mock().expectOneCall("can_class->read")
          .andReturnValue(-1);
    can->read();
}

TEST(CAN_arduino_driver, get_packet_id) {
    long PACKET_ID = 99;
    mock().expectOneCall("can_class->packetId")
          .andReturnValue(PACKET_ID);
    
    CHECK_EQUAL(PACKET_ID, can->get_packet_id());
}