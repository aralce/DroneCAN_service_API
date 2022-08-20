#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <canard.h>
#include <uavcan.protocol.NodeStatus.h>
#include <uavcan.protocol.SoftwareVersion.h>
#include <uavcan.protocol.HardwareVersion.h>
#include <common_to_all_mocks.h>

#define UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_MAX_SIZE 377
#define UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_SIGNATURE (0xEE468A8121C46A9EULL)
#define UAVCAN_PROTOCOL_GETNODEINFO_RESPONSE_ID 1

struct uavcan_protocol_GetNodeInfoResponse {
    struct uavcan_protocol_NodeStatus status;
    struct uavcan_protocol_SoftwareVersion software_version;
    struct uavcan_protocol_HardwareVersion hardware_version;
    struct { uint8_t len; uint8_t data[80]; }name;
};

class Uavcan_protocol_getNodeInfoResponse_comparator : public MockNamedValueComparator
{
public:
    virtual bool isEqual(const void* object1, const void* object2)
    {
        uavcan_protocol_GetNodeInfoResponse* msg_1 = (uavcan_protocol_GetNodeInfoResponse*)object1;
        uavcan_protocol_GetNodeInfoResponse* msg_2 = (uavcan_protocol_GetNodeInfoResponse*)object2;

        if (are_data_pointer_equal((uint8_t*)msg_1, (uint8_t*)msg_2, sizeof(uavcan_protocol_GetNodeInfoResponse)))
            return true;
        else
            return false;
    }

    virtual SimpleString valueToString(const void* type_info)
    {
        return StringFrom(type_info);
    }
};


inline uint32_t uavcan_protocol_GetNodeInfoResponse_encode(struct uavcan_protocol_GetNodeInfoResponse* msg, uint8_t* buffer) {
    mock().actualCall("uavcan_protocol_GetNodeInfoResponse_encode")
          .withParameter("msg" , (const void*)msg)
          .withOutputParameter("buffer", (void*)buffer);
    return mock().returnUnsignedIntValueOrDefault(0);
}