#pragma once
#include <CAN.h>

typedef struct {
    uint8_t* pointer;
    size_t length;
}CAN_data_t;

typedef struct {
    long id;
    CAN_data_t data;
}CAN_bus_driver_fame_t;

class CAN_bus_adaptor {
public:
    CAN_bus_adaptor() {

    }

    int begin(long baudRate) {
        return can.begin(baudRate);
    }

    void setPins(int rx, int tx) {
        can.setPins(rx, tx);
    }

    bool send_frame(CAN_bus_driver_fame_t can_frame) {
        bool is_success = can.beginExtendedPacket(can_frame.id);
        can.write(can_frame.data.pointer, can_frame.data.length);
        is_success &= can.endPacket();
        return is_success;
    }
};