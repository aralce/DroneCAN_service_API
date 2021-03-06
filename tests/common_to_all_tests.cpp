#include "common_to_all_tests.h"

void teardown_mocks()
{
    mock().checkExpectations();
    mock().clear();
}

void execute_test_teardown() {
    teardown_mocks();
    mock().removeAllComparatorsAndCopiers();
}

void dummy_function(DroneCAN_error) {}
DroneCAN_service get_droneCAN_instance_omiting_mock_calls(droneCAN_handle_error_t handle_error)
{
    mock().disable();
    DroneCAN_service droneCAN_service(handle_error);
    mock().enable();
    return droneCAN_service;
}