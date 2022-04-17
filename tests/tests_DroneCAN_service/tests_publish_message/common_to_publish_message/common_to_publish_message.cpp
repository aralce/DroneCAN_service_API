#include "common_to_publish_message.h"

void dummy_function(DroneCAN_error) {}
DroneCAN_service get_droneCAN_instance_omiting_mock_calls(droneCAN_handle_error_t handle_error)
{
    mock().disable();
    DroneCAN_service droneCAN_service(handle_error);
    mock().enable();
    return droneCAN_service;
}

void publish_batteryInfo_message(DroneCAN_service &droneCAN_service)
{
    uavcan_equipment_power_BatteryInfo batteryInfo;
    droneCAN_service.publish_message(batteryInfo); 
}

void execute_test_teardown() {
    teardown_mocks();
    mock().removeAllComparatorsAndCopiers();
}