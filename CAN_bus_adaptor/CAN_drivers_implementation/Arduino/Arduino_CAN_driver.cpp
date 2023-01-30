#include "Arduino_CAN_driver.h"
#ifndef IS_RUNNING_TESTS
    #include <CAN.h>
#else
    #include <CAN_BUS_adaptor/CAN_driver_implementation/Arduino_CAN_driver/CAN.h>
#endif

bool Arduino_CAN_driver::begin(long baudRate) {
    return CAN.begin(baudRate);
}

void Arduino_CAN_driver::setPins(int rx, int tx) {
    CAN.setPins(rx, tx);
}

bool Arduino_CAN_driver::send_frame(CanardCANFrame& can_frame) {
    bool is_success = CAN.beginExtendedPacket(can_frame.id & CANARD_CAN_EXT_ID_MASK);
    is_success &= try_CAN_write(can_frame.data, can_frame.data_len);
    is_success &= CAN.endPacket() > 0;
    return is_success;
}

void Arduino_CAN_driver::onReceive(void (*onReceive_callback)(int packet_size)) {
    CAN.onReceive(onReceive_callback);
}

int Arduino_CAN_driver::read() {
    return CAN.read();
}

long Arduino_CAN_driver::get_packet_id() {
    return CAN.packetId();
}

bool Arduino_CAN_driver::try_CAN_write(const uint8_t* buffer, size_t size) {
    size_t bytes_written = CAN.write(buffer, size);
    bool was_writing_successful = (bytes_written == size);
    return was_writing_successful;
}
