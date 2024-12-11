#include "common_to_DroneCAN_service_tests.h"

void teardown_mocks()
{
    mock().checkExpectations();
    mock().clear();
}

void execute_test_teardown() {
    teardown_mocks();
    mock().removeAllComparatorsAndCopiers();
}

static CAN_driver_interface can_driver;

DroneCAN_service get_droneCAN_instance_omiting_mock_calls(droneCAN_handle_error_t handle_error)
{
    mock().disable();
    DroneCAN_service droneCAN_service(can_driver, DEFAULT_NODE_ID,
                                      handle_error);
    mock().enable();
    return droneCAN_service;
}