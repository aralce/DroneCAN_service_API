#include "common_to_DroneCAN_service_tests.h"
#include <CAN_bus_adaptor_factory.h>

void teardown_mocks()
{
    mock().checkExpectations();
    mock().clear();
    CAN_bus_adapter_singleton::clear_instances();
}

void execute_test_teardown() {
    teardown_mocks();
    mock().removeAllComparatorsAndCopiers();
}

DroneCAN_service get_droneCAN_instance_omiting_mock_calls(droneCAN_handle_error_t handle_error)
{
    mock().disable();
    DroneCAN_service droneCAN_service(DEFAULT_NODE_ID, handle_error);
    mock().enable();
    return droneCAN_service;
}