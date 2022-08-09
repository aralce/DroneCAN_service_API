#include "uavcan.protocol.param.GetSet_res.h"

uint32_t uavcan_protocol_param_GetSetResponse_encode(struct uavcan_protocol_param_GetSetResponse* msg, uint8_t* buffer) {
    mock().actualCall("uavcan_protocol_param_GetSetResponse_encode")
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "msg", (const void*)msg);
    return mock().returnUnsignedIntValueOrDefault(0);
}