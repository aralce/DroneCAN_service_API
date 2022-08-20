#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <canard.h>

#define UAVCAN_PROTOCOL_HARDWAREVERSION_MAX_SIZE 274
#define UAVCAN_PROTOCOL_HARDWAREVERSION_SIGNATURE (0xAD5C4C933F4A0C4ULL)

struct uavcan_protocol_HardwareVersion {
    uint8_t major;
    uint8_t minor;
    uint8_t unique_id[16];
    struct { uint8_t len; uint8_t data[255]; }certificate_of_authenticity;
};