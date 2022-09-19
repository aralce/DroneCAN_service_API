#ifdef TARGET_RP2040

#pragma once
#include <canard.h>
#include <lib/libraries_for_RP2040/can2040/src/can2040.h>
#include <DroneCAN_service_configuration.h>
class CAN_bus_adaptor {
public:
    CAN_bus_adaptor() {

    }

    bool begin(long baudRate);

    void setPins(int rx, int tx);

    bool send_frame(CanardCANFrame& can_frame);

    void onReceive(void (*onReceive_callback)(int packet_size));

    int read();
    
    long get_packet_id();

private:
    uint32_t can_bus_rx_pin = CAN_BUS_CRX_PIN;
    uint32_t can_bus_tx_pin = CAN_BUS_CTX_PIN;

    uint32_t bitrate;
};

#endif // TARGET_RP2040