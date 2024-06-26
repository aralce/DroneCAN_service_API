#include "uavcan.protocol.param.GetSet_req.h"
#include <common_to_all_mocks.h>
#include <cstring>

uint32_t uavcan_protocol_param_GetSetRequest_encode(struct uavcan_protocol_param_GetSetRequest* msg, uint8_t* buffer)
{
    return 0;
}

bool uavcan_protocol_param_GetSetRequest_decode(const CanardRxTransfer* transfer, struct uavcan_protocol_param_GetSetRequest* msg)
{
    unsigned char transfer_buffer[sizeof(CanardRxTransfer)];
    memcpy(transfer_buffer, transfer, sizeof(CanardRxTransfer));

    mock().actualCall("uavcan_protocol_param_GetSetRequest_decode")
          .withMemoryBufferParameter("transfer", transfer_buffer, sizeof(CanardRxTransfer))
          .withOutputParameter("msg", (void*)msg);
    return true;
}