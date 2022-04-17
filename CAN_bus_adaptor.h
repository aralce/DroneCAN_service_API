#pragma once
#include <CAN.h>

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

    bool send_frame(CanardCANFrame& can_frame) {
        bool is_success = can.beginExtendedPacket(can_frame.id);
        can.write(can_frame.data, sizeof(can_frame.data));
        is_success &= can.endPacket();
        return is_success;
    }
};
