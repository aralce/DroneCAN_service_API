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
};
