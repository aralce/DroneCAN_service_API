#pragma once
#include <stdbool.h>
#include "CAN_bus_adaptor/CAN_bus_adaptor.h"
#include <canard.h>

class ESP_IDF_CAN_driver: public CAN_bus_adaptor {
public:
    ~ESP_IDF_CAN_driver();

    bool begin(CAN_bitrate baudRate) override;

    void setPins(int rx, int tx) override;

    bool send_frame(CanardCANFrame& can_frame) override;

    void onReceive(void (*onReceive_callback)(int packet_size)) override;

    int read() override;

    long get_packet_id() override;

private:
    int tx_pin = 5;
    int rx_pin = 4;

    static const int MAX_BYTES_PER_FRAME = 8;
    uint8_t received_frame_buffer[MAX_BYTES_PER_FRAME] = {};
    uint8_t index_of_byte_read_from_buffer = 0;

    bool is_driver_running = false;
    CAN_bitrate _bitrate = CAN_bitrate::CAN_1MBITS;

    bool try_CAN_write(const uint8_t* buffer, size_t size);
};