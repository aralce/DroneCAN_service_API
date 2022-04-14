#pragma once
#include <CAN.h>

class CAN_bus_interface {
public:
    int begin(long baudRate) {
        return can.begin(baudRate);
    }

    void setPins(int rx, int tx) {
        can.setPins(rx, tx);
    }    
};
