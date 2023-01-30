#pragma once
#include "CAN_bus_adaptor/CAN_bus_adaptor.h"
#include <canard.h>

class Arduino_CAN_driver: public CAN_bus_adaptor {
public:
    bool begin(long baudRate) override;

    void setPins(int rx, int tx) override;

    bool send_frame(CanardCANFrame& can_frame) override;

    void onReceive(void (*onReceive_callback)(int packet_size)) override;

    int read() override;

    long get_packet_id() override;

private:
    bool try_CAN_write(const uint8_t* buffer, size_t size);
};