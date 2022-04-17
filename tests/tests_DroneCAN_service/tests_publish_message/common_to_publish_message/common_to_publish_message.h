#pragma once
#include <common_to_all_tests.h>

void dummy_function(DroneCAN_error);
DroneCAN_service get_droneCAN_instance_omiting_mock_calls(droneCAN_handle_error_t handle_error = dummy_function);

void publish_batteryInfo_message(DroneCAN_service &droneCAN_service);

void execute_test_teardown();