#include <uavcan.protocol.NodeStatus.h>

uint32_t uavcan_protocol_NodeStatus_encode(struct uavcan_protocol_NodeStatus* msg, uint8_t* buffer) {
    mock().actualCall("uavcan_protocol_NodeStatus_encode")
          .withParameterOfType("uavcan_protocol_NodeStatus", "msg", (const void*)msg)
          .withOutputParameter("buffer", buffer);
    return mock().unsignedIntReturnValue(); 
}