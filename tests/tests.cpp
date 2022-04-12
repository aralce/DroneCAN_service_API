#include "CppUTest/TestHarness.h"
#include "DroneCan_service_singleton.h"

TEST_GROUP(DroneCan_service)
{
};

/*
- DroneCan_Service_singleton a batteryInfo message is published
- if a batteryInfo message is published and there is no queue message. The message is dispatched.*/

TEST(DroneCan_service, instance_is_get_from_singleton_twice)
{
	DroneCan_service& droneCan_service = DroneCan_service::getInstance();
	DroneCan_service& droneCan_service2 = DroneCan_service::getInstance();
	CHECK_EQUAL(&droneCan_service, &droneCan_service2);
}

// TEST(DroneCan_service, batteryInfo_message_is_published)
// {
// 	DroneCan_service& droneCan_service = DroneCan_service::getInstance();
// 	CHECK_EQUAL(errorCode::OK, droneCan_service.pusblish())
// }