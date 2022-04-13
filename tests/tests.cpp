#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "DroneCan_service.h"

TEST_GROUP(DroneCan_service)
{

};

// - When DroneCan_service inits the libcanard library is initialized.
// - When DroneCan_service inits the can bus is initialized.
// - batteryInfo message is publised once.

TEST(DroneCan_service, on_initialization_libcanard_is_initialized) {
	mock().expectOneCall("initCanard");
	mock().ignoreOtherCalls();
	DroneCan_service service;
	mock().checkExpectations();
}