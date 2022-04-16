#include "DroneCAN_service.h"
#include "DroneCAN_service_configuration.h"

static void handle_reception_DUMMY(CanardInstance* ins, CanardRxTransfer* transfer) {}
static bool handle_acceptance_DUMMY(const CanardInstance* ins, uint64_t* out_data_type_signature, uint16_t data_type_id, CanardTransferType transfer_type, uint8_t source_node_id) {return false;}

static void initialize_CAN_bus_driver(CAN_bus_adaptor& can_driver);
bool is_CAN_bus_initialization_successful = false;

DroneCAN_service::DroneCAN_service() {
    canard.init(handle_reception_DUMMY, handle_acceptance_DUMMY);
    initialize_CAN_bus_driver(can_driver);
    _is_healthy = is_CAN_bus_initialization_successful;
}

static void initialize_CAN_bus_driver(CAN_bus_adaptor& can_driver) {
    can_driver.setPins(CAN_BUS_RX_PIN, CAN_BUS_TX_PIN);
    is_CAN_bus_initialization_successful = can_driver.begin(CAN_BUS_BAUDRATE);
}

bool DroneCAN_service::is_healthy() {
    return _is_healthy;
}

void DroneCAN_service::publish_message(uavcan_equipment_power_BatteryInfo& battery_info) {
    uint8_t buffer[UAVCAN_EQUIPMENT_POWER_BATTERYINFO_MAX_SIZE];
    uint32_t message_length = uavcan_equipment_power_BatteryInfo_encode(&battery_info, buffer);
    
    canard_message_type_info_t type_info = {.signature = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_SIGNATURE,
                                            .id = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_ID,
                                            .priority = CANARD_TRANSFER_PRIORITY_LOW };
    canard_message_data_t data = {.ptr = buffer,
                                  .length = (uint16_t)message_length };
    canard.broadcast(type_info, data);
}
