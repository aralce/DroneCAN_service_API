#include "DroneCAN_service.h"
#include "DroneCAN_service_configuration.h"
#include <canard.h>
void on_canardReceptionDummy(CanardInstance* ins, CanardRxTransfer* transfer) {}
bool canardShouldAcceptTransferDummy(const CanardInstance* ins, uint64_t* out_data_type_signature, uint16_t data_type_id, CanardTransferType transfer_type, uint8_t source_node_id) {}

static void libcanard_init();

DroneCAN_service::DroneCAN_service() {
	libcanard_init();
}

static void libcanard_init() {
	static CanardInstance canardInstance;
	static uint8_t canardBuffer[LIBCANARD_ALLOCATION_BUFFER_IN_BYTES];
	canardInit(&canardInstance, canardBuffer, LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, on_canardReceptionDummy, canardShouldAcceptTransferDummy, NULL);
}
