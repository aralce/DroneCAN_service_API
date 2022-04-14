#include "DroneCAN_service.h"
#include "DroneCAN_service_configuration.h"

static void on_canardReception_DUMMY(CanardInstance* ins, CanardRxTransfer* transfer) {}
static bool canardShouldAcceptTransfer_DUMMY(const CanardInstance* ins,
											 uint64_t* out_data_type_signature,
											 uint16_t data_type_id,
											 CanardTransferType transfer_type,
											 uint8_t source_node_id) {return false;}

static void initialize_CAN_bus(CAN_bus_interface& can_interface);
bool is_CAN_bus_initialization_successful = false;

DroneCAN_service::DroneCAN_service() {
	canardInit(&canardInstance, canardBuffer, LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, on_canardReception_DUMMY, canardShouldAcceptTransfer_DUMMY, NULL);
	initialize_CAN_bus(can_interface);
	_is_healthy = is_CAN_bus_initialization_successful;
}

static void initialize_CAN_bus(CAN_bus_interface& can_interface) {
	can_interface.setPins(CAN_BUS_RX_PIN, CAN_BUS_TX_PIN);
	is_CAN_bus_initialization_successful = can_interface.begin(CAN_BUS_BAUDRATE);
}

bool DroneCAN_service::is_healthy() {
	return _is_healthy;
}
