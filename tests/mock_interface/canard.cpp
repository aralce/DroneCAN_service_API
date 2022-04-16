#include <common_to_all_mocks.h>
#include <canard.h>

void canardInit(CanardInstance* out_ins, void* mem_arena, size_t mem_arena_size, CanardOnTransferReception on_reception, CanardShouldAcceptTransfer should_accept, void* user_reference)
{
    mock().actualCall("canardInit");
}

int16_t canardBroadcast(CanardInstance* ins, uint64_t data_type_signature, uint16_t data_type_id, uint8_t* inout_transfer_id, uint8_t priority, const void* payload, uint16_t payload_len) {
    mock().actualCall("canardBroadcast")
          .withParameter("data_type_signature", data_type_signature)
          .withParameter("data_type_id", data_type_id)
          .withParameter("priority", priority)
          .withPointerParameter("payload", (void*)payload)
          .withParameter("payload_len", payload_len);
    return mock().intReturnValue();
}