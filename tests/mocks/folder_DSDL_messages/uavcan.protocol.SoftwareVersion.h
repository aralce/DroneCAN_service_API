#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <canard.h>

#define UAVCAN_PROTOCOL_SOFTWAREVERSION_MAX_SIZE 15
#define UAVCAN_PROTOCOL_SOFTWAREVERSION_SIGNATURE (0xDD46FD376527FEA1ULL)

#define UAVCAN_PROTOCOL_SOFTWAREVERSION_OPTIONAL_FIELD_FLAG_VCS_COMMIT 1
#define UAVCAN_PROTOCOL_SOFTWAREVERSION_OPTIONAL_FIELD_FLAG_IMAGE_CRC 2

struct uavcan_protocol_SoftwareVersion {
    uint8_t major;
    uint8_t minor;
    uint8_t optional_field_flags;
    uint32_t vcs_commit;
    uint64_t image_crc;
};