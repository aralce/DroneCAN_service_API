#pragma once
#include <CAN.h>

class CAN_bus_adaptor {
public:
    CAN_bus_adaptor() {

    }

    int begin(long baudRate) {
        return CAN.begin(baudRate);
    }

    void setPins(int rx, int tx) {
        CAN.setPins(rx, tx);
    }

    bool send_frame(CanardCANFrame& can_frame) {
        bool is_success = CAN.beginExtendedPacket(can_frame.id & CANARD_CAN_EXT_ID_MASK);
        CAN.write(can_frame.data, can_frame.data_len);
        is_success &= CAN.endPacket();
        return is_success;
    }
};
