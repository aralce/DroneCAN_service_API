#include <uavcan.protocol.NodeStatus.h>
#include <common_to_all_mocks.h>

uint32_t uavcan_protocol_NodeStatus_encode(struct uavcan_protocol_NodeStatus* msg, uint8_t* buffer) {
    mock().actualCall("uavcan_protocol_NodeStatus_encode")
          .withParameter("msg", msg)
          .withOutputParameter("buffer", buffer);
    return mock().unsignedIntReturnValue(); 
}