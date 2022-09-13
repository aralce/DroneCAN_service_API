#pragma once
#include <CAN.h>
#include <canard.h>

class CAN_bus_adaptor {
public:
    CAN_bus_adaptor() {

    }

    bool begin(long baudRate) {
        return CAN.begin(baudRate);
    }

    void setPins(int rx, int tx) {
        CAN.setPins(rx, tx);
    }

    bool send_frame(CanardCANFrame& can_frame) {
        bool is_success = CAN.beginExtendedPacket(can_frame.id & CANARD_CAN_EXT_ID_MASK);
        is_success &= try_CAN_write(can_frame.data, can_frame.data_len);
        is_success &= CAN.endPacket() > 0;
        return is_success;
    }

    void onReceive(void (*onReceive_callback)(int packet_size)) {
        CAN.onReceive(onReceive_callback);
    }

    int read() {
        return CAN.read();
    }

    long get_packet_id() {return CAN.packetId();}

private:
    bool try_CAN_write(const uint8_t* buffer, size_t size) {
        size_t bytes_written = CAN.write(buffer, size);
        bool was_writing_successful = (bytes_written == size);
        return was_writing_successful;
    }
};