#include <common_to_DroneCAN_service_tests.h>

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

TEST(DroneCAN_service_initialization, system_inits_healthy) {
    DroneCAN_service droneCAN_service = get_DroneCAN_ignoring_other_calls();

    CHECK_TRUE(droneCAN_service.is_healthy());
}

// TEST(DroneCAN_service_initialization, on_initialization_DroneCAN_message_sender_is_init)
// {
//     mock().expectOneCall("DroneCAN_message_sender->constructor")
//           .ignoreOtherParameters();
//     mock().ignoreOtherCalls();
//     DroneCAN_service droneCAN_service;
// }

TEST(DroneCAN_service_initialization, on_initialization_libcanard_is_initialized)
{
    mock().expectOneCall("canard->init");
    mock().ignoreOtherCalls();
    DroneCAN_service dronceCAN_service;
}

TEST(DroneCAN_service_initialization, if_node_ID_is_not_provided_inits_with_default_ID) {
    mock().expectOneCall("canard->set_node_ID")
          .withUnsignedIntParameter("self_node_id", DEFAULT_NODE_ID);
    
    DroneCAN_service droneCAN_service = get_DroneCAN_ignoring_other_calls();
    CHECK_EQUAL(DEFAULT_NODE_ID, droneCAN_service.get_node_ID());
}

TEST(DroneCAN_service_initialization, node_ID_is_provided) {
    const uint8_t NODE_ID = 50;
    mock().expectOneCall("canard->set_node_ID")
          .withUnsignedIntParameter("self_node_id", NODE_ID);
    mock().ignoreOtherCalls();
    
    DroneCAN_service droneCAN_service(NODE_ID);
    CHECK_EQUAL(NODE_ID, droneCAN_service.get_node_ID());
}

TEST(DroneCAN_service_initialization, on_initialization_CAN_BUS_is_initialized)
{
    mock().expectOneCall("CAN_bus_adaptor->begin")
          .withParameter("baudRate", CAN_BUS_BAUDRATE)
          .andReturnValue(INITIALIZATION_SUCCESSFUL);   
    
    mock().expectOneCall("CAN_bus_adaptor->setPins").withParameter("rx", CAN_BUS_CRX_PIN)
          .withParameter("tx", CAN_BUS_CTX_PIN);   
    
    DroneCAN_service droneCAN_service{get_DroneCAN_ignoring_other_calls()};
}

TEST(DroneCAN_service_initialization, failed_initialization_system_is_unhealthy)
{
    mock().expectOneCall("CAN_bus_adaptor->begin")
          .withParameter("baudRate", CAN_BUS_BAUDRATE)
          .andReturnValue(FAILURE_IN_INITIALIZATION);
    
    DroneCAN_service droneCAN_service{get_DroneCAN_ignoring_other_calls()};
    CHECK_FALSE(droneCAN_service.is_healthy());
}

extern void DUMMY_error_handler(DroneCAN_error error);

TEST(DroneCAN_service_initialization, droneCAN_without_handle_error_function)
{
    mock().ignoreOtherCalls();
    Spied_droneCAN_service droneCAN_service;
    
    droneCAN_handle_error_t handle_error = droneCAN_service.get_pointer_to_handle_error_function();
    
    POINTERS_EQUAL(DUMMY_error_handler , handle_error);
}

void handle_error_function(DroneCAN_error error) {}

TEST(DroneCAN_service_initialization, droneCAN_with_handle_error_function)
{
    mock().ignoreOtherCalls();
    Spied_droneCAN_service droneCAN_service(DEFAULT_NODE_ID, handle_error_function);
    droneCAN_handle_error_t handle_error = droneCAN_service.get_pointer_to_handle_error_function();
    
    POINTERS_EQUAL(handle_error_function, handle_error);
}

void handle_error_init_fail(DroneCAN_error error)
{
    mock().actualCall("handle_error_init_fail")
          .withIntParameter("error", (int)error);
} 

TEST(DroneCAN_service_initialization, when_fails_the_error_to_handle_is_ON_INITIALIZATION)
{
    mock().expectOneCall("CAN_bus_adaptor->begin")
          .ignoreOtherParameters()
          .andReturnValue(FAILURE_IN_INITIALIZATION);

    mock().expectOneCall("handle_error_init_fail")
          .withParameter("error", (int)DroneCAN_error::ON_INITIALIZATION);
    
    mock().ignoreOtherCalls();
    DroneCAN_service droneCAN_service(DEFAULT_NODE_ID, handle_error_init_fail);
}