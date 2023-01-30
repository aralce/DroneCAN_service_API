#pragma once
#include <canard.h>

class CAN_bus_adaptor {
public:
    virtual ~CAN_bus_adaptor() {}

    virtual bool begin(long baudRate) = 0;
    virtual void setPins(int rx, int tx) = 0;

    virtual bool send_frame(CanardCANFrame& can_frame) = 0;

    virtual void onReceive(void (*onReceive_callback)(int packet_size)) = 0;

    virtual int read() = 0;

    virtual long get_packet_id() = 0;
};
