#pragma once
#include <canard.h>

class CAN_bus_adaptor {
public:
    CAN_bus_adaptor() {

    }

    bool begin(long baudRate) {
        return true;
    }

    void setPins(int rx, int tx) {
        
    }

    bool send_frame(CanardCANFrame& can_frame) {
        return true;
    }

    void onReceive(void (*onReceive_callback)(int packet_size)) {

    }

    int read() {
        return 0;
    }

    long get_packet_id() {return 2;}

};