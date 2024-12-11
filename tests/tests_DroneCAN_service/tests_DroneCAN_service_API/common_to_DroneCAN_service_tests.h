#pragma once 
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <DroneCAN_service_API.h>
#include <DroneCAN_service_spy.h>
#include <DroneCAN_service_configuration.h>

#define FAILURE_IN_INITIALIZATION false
#define INITIALIZATION_SUCCESSFUL true

void teardown_mocks();

void execute_test_teardown();

DroneCAN_service get_droneCAN_instance_omiting_mock_calls(droneCAN_handle_error_t handle_error = nullptr);