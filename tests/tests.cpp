#include "common_to_all_tests.h"

TEST_GROUP(DroneCAN_service)
{
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(DroneCAN_service, system_is_healthy) {
	const int INITIALIZATION_SUCCESSFUL = 1;
	mock().expectOneCall("begin").withParameter("baudRate", CAN_BUS_BAUDRATE).andReturnValue(INITIALIZATION_SUCCESSFUL);
	mock().ignoreOtherCalls();
	DroneCAN_service service;
	CHECK_TRUE(service.is_healthy());
}

