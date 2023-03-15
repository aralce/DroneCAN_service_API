#pragma once
#include <canard.h>

enum class CAN_bitrate {CAN_1MBITS, CAN_800KBITS, CAN_500KBITS, CAN_250KBITS, CAN_125KBITS, CAN_100KBITS, CAN_50KBITS, CAN_25KBITS};

class CAN_bus_adaptor {
public:
    virtual ~CAN_bus_adaptor() {}

    virtual bool begin(CAN_bitrate bitrate) = 0;
    virtual void setPins(int rx, int tx) = 0;

    virtual bool send_frame(CanardCANFrame& can_frame) = 0;

    virtual void onReceive(void (*onReceive_callback)(int packet_size)) = 0;

    virtual int read() = 0;

    virtual long get_packet_id() = 0;

protected:
    long CAN_enum_class_to_number(CAN_bitrate bitrate) const {
        switch (bitrate) {
            case CAN_bitrate::CAN_1MBITS:
                return 1e6;
            case CAN_bitrate::CAN_800KBITS:
                return 800e3;
            case CAN_bitrate::CAN_500KBITS:
                return 500e3;
            case CAN_bitrate::CAN_250KBITS:
                return 250e3;
            case CAN_bitrate::CAN_125KBITS:
                return 125e3;
            case CAN_bitrate::CAN_100KBITS:
                return 100e3;
            case CAN_bitrate::CAN_50KBITS:
                return 50e3;
            case CAN_bitrate::CAN_25KBITS:
                return 25e3;
            default:
                return 1e6;
        }
    }
};
