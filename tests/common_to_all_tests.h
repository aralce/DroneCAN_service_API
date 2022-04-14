#pragma once 
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DroneCAN_service.h"

#define FAILURE_IN_INITIALIZATION 0
#define INITIALIZATION_SUCCESSFUL 1

void teardown_mocks();