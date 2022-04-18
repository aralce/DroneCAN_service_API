#include <common_to_all_tests.h>
#include <DroneCAN_service_configuration.h>

DroneCAN_service get_DroneCAN_ignoring_other_calls()
{
    mock().ignoreOtherCalls();
    return DroneCAN_service();
}

TEST_GROUP(DroneCAN_service_initialization)
{
    void teardown()
    {
        teardown_mocks();
    }
};

TEST(DroneCAN_service_initialization, system_init_healthy) {
    DroneCAN_service DroneCAN_service = get_DroneCAN_ignoring_other_calls();
    CHECK_TRUE(DroneCAN_service.is_healthy());
}

TEST(DroneCAN_service_initialization, droneCAN_without_handle_error_function)
{
    mock().ignoreOtherCalls();
    Spied_droneCAN_service droneCAN_service;
    
    droneCAN_handle_error_t handle_error = droneCAN_service.get_pointer_to_handle_error_function();
    droneCAN_handle_error_t handle_error_DUMMY = droneCAN_service.get_handle_error_DUMMY();
    
    POINTERS_EQUAL(handle_error_DUMMY , handle_error);
}


void handle_error_function(DroneCAN_error error) {}

TEST(DroneCAN_service_initialization, droneCAN_with_handle_error_function)
{
    mock().ignoreOtherCalls();
    Spied_droneCAN_service droneCAN_service(handle_error_function);
    droneCAN_handle_error_t handle_error = droneCAN_service.get_pointer_to_handle_error_function();
    
    POINTERS_EQUAL(handle_error_function, handle_error);
}


TEST(DroneCAN_service_initialization, on_initialization_libcanard_is_initialized)
{
    mock().expectOneCall("init");
    DroneCAN_service dronceCAN_service{get_DroneCAN_ignoring_other_calls()};
}


TEST(DroneCAN_service_initialization, on_initialization_CAN_BUS_is_initialized)
{
    mock().expectOneCall("begin")
          .withParameter("baudRate", CAN_BUS_BAUDRATE)
          .andReturnValue(INITIALIZATION_SUCCESSFUL);   
    
    mock().expectOneCall("setPins").withParameter("rx", CAN_BUS_RX_PIN)
          .withParameter("tx", CAN_BUS_TX_PIN);   
    
    DroneCAN_service droneCAN_service{get_DroneCAN_ignoring_other_calls()};
}


TEST(DroneCAN_service_initialization, failed_initialization_system_is_unhealthy)
{
    mock().expectOneCall("begin")
          .withParameter("baudRate", CAN_BUS_BAUDRATE)
          .andReturnValue(FAILURE_IN_INITIALIZATION);
    
    DroneCAN_service droneCAN_service{get_DroneCAN_ignoring_other_calls()};
    CHECK_FALSE(droneCAN_service.is_healthy());
}


void handle_error_init_fail(DroneCAN_error error)
{
    mock().actualCall("handle_error_init_fail")
          .withIntParameter("error", (int)error);
} 

TEST(DroneCAN_service_initialization, when_fails_the_error_to_handle_is_ON_INITIALIZATION)
{
    mock().expectOneCall("begin")
          .ignoreOtherParameters()
          .andReturnValue(FAILURE_IN_INITIALIZATION);

    mock().expectOneCall("handle_error_init_fail")
          .withParameter("error", (int)DroneCAN_error::ON_INITIALIZATION);
    
    mock().ignoreOtherCalls();
    DroneCAN_service droneCAN_service(handle_error_init_fail);
}