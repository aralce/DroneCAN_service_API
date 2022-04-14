#include "common_to_all_tests.h"
#include "DroneCAN_service_configuration.h"

TEST_GROUP(DroneCAN_service_initialization)
{
	void teardown() {
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(DroneCAN_service_initialization, on_initialization_libcanard_is_initialized) {
	mock().expectOneCall("canardInit");
	mock().ignoreOtherCalls();
	DroneCAN_service service;
}

TEST(DroneCAN_service_initialization, on_initialization_CAN_BUS_is_initialized) {
	mock().expectOneCall("begin").withParameter("baudRate", CAN_BUS_BAUDRATE).andReturnValue(1);
	mock().expectOneCall("setPins").withParameter("rx", CAN_BUS_RX_PIN).withParameter("tx", CAN_BUS_TX_PIN);
	mock().ignoreOtherCalls();
	DroneCAN_service service;
}

TEST(DroneCAN_service_initialization, failed_initialization_system_is_unhealthy) {
	const int FAILURE_IN_INITIALIZATION = 0;
	mock().expectOneCall("begin").withParameter("baudRate", CAN_BUS_BAUDRATE).andReturnValue(FAILURE_IN_INITIALIZATION);
	mock().ignoreOtherCalls();
	DroneCAN_service service;
	CHECK_FALSE(service.is_healthy());
}