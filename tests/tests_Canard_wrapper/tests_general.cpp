#include <common_to_all_tests.h>
#include <Canard_wrapper.h>
#include <DroneCAN_service_configuration.h>

Canard canard(LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, UAVCAN_MAX_BYTES_ON_MESSAGE);

TEST_GROUP(Canard_wrapper)
{

};


TEST(Canard_wrapper, init)
{
    // mock().expectOneCall("canardInit")
}