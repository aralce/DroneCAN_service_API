#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <canard.h>
#include <uavcan.protocol.NodeStatus.h>
#include <uavcan.protocol.SoftwareVersion.h>
#include <uavcan.protocol.HardwareVersion.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE 377
#define UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_SIGNATURE (0xEE468A8121C46A9EULL)
#define UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_ID 1

struct uavcan_protocol_GetNodeInfoResponse {
    struct uavcan_protocol_NodeStatus status;
    struct uavcan_protocol_SoftwareVersion software_version;
    struct uavcan_protocol_HardwareVersion hardware_version;
    struct { uint8_t len; uint8_t data[80]; }name;
};

inline uint32_t uavcan_protocol_GetNodeInfoResponse_encode(struct uavcan_protocol_GetNodeInfoResponse* msg, uint8_t* buffer) {return 0;}