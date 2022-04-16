#include "DroneCAN_service.h"
#include "DroneCAN_service_configuration.h"

static void initialize_CAN_bus_driver(CAN_bus_adaptor& can_driver);
bool is_CAN_bus_initialization_successful = false;

DroneCAN_service::DroneCAN_service(droneCAN_handle_error_t handle_error) : _handle_error(handle_error) {
    canard.init();
    try_initialize_CAN_bus_driver();
    _is_healthy = is_CAN_bus_initialization_successful;
}

void DroneCAN_service::try_initialize_CAN_bus_driver() {
    initialize_CAN_bus_driver(can_driver);
    if (!is_CAN_bus_initialization_successful)
        _handle_error(DroneCAN_error::ON_INITIALIZATION);
}

static void initialize_CAN_bus_driver(CAN_bus_adaptor& can_driver) {
    can_driver.setPins(CAN_BUS_RX_PIN, CAN_BUS_TX_PIN);
    is_CAN_bus_initialization_successful = can_driver.begin(CAN_BUS_BAUDRATE);
}

bool DroneCAN_service::is_healthy() {
    return _is_healthy;
}

void DroneCAN_service::publish_message(uavcan_equipment_power_BatteryInfo& battery_info) {
    uint8_t buffer[UAVCAN_EQUIPMENT_POWER_BATTERYINFO_MAX_SIZE]{};
    uint32_t message_length = uavcan_equipment_power_BatteryInfo_encode(&battery_info, buffer);
    
    canard_message_type_info_t type_info = {.signature = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_SIGNATURE,
                                            .id = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_ID,
                                            .priority = CANARD_TRANSFER_PRIORITY_LOW };
    canard_message_data_t data = {.ptr = (void*)buffer,
                                  .length = (uint16_t)message_length };
    canard.broadcast(type_info, data);
}
