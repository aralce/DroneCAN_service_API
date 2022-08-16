#include "uavcan.protocol.param.GetSet_req.h"
#include <common_to_all_mocks.h>

uint32_t uavcan_protocol_param_GetSetRequest_encode(struct uavcan_protocol_param_GetSetRequest* msg, uint8_t* buffer) {
    return 0;
}

bool uavcan_protocol_param_GetSetRequest_decode(const CanardRxTransfer* transfer, struct uavcan_protocol_param_GetSetRequest* msg) {
    mock().actualCall("uavcan_protocol_param_GetSetRequest_decode")
          .withPointerParameter("transfer", (void*)transfer)
          .withOutputParameter("msg", (void*)msg);
    return true;
}