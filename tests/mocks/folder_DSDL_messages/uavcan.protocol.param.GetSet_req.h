#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <canard.h>
#include <uavcan.protocol.param.Value.h>

#define UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_MAX_SIZE 224
#define UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_SIGNATURE (0xA7B622F939D1A4D5ULL)
#define UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_ID 11

struct uavcan_protocol_param_GetSetRequest {
    uint16_t index;
    struct uavcan_protocol_param_Value value;
    struct { uint8_t len; uint8_t data[92]; }name;
};

uint32_t uavcan_protocol_param_GetSetRequest_encode(struct uavcan_protocol_param_GetSetRequest* msg, uint8_t* buffer);
bool uavcan_protocol_param_GetSetRequest_decode(const CanardRxTransfer* transfer, struct uavcan_protocol_param_GetSetRequest* msg);