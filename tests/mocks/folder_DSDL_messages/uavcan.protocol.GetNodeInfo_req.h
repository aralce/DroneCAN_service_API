#pragma once
#include <common_to_all_mocks.h>
#include <stdbool.h>
#include <stdint.h>
#include <canard.h>

#define UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_MAX_SIZE 0
#define UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_SIGNATURE (0xEE468A8121C46A9EULL)
#define UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_ID 1

struct uavcan_protocol_GetNodeInfoRequest {
};

inline bool uavcan_protocol_GetNodeInfoRequest_decode(const CanardRxTransfer* transfer, struct uavcan_protocol_GetNodeInfoRequest* msg) {
    mock().actualCall("uavcan_protocol_GetNodeInfoRequest_decode")
          .withPointerParameter("transfer", (void*)transfer)
          .withOutputParameter("msg", (void*)msg);
    return true;
}

