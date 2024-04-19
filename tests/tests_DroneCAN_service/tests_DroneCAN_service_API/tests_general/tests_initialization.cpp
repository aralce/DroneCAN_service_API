#include <common_to_DroneCAN_service_tests.h>
#include <support/DroneCAN_service_spy.h>

CAN_bus_custom* custom_can = nullptr;
static CAN_bus_adaptor can_driver;

DroneCAN_service get_DroneCAN_ignoring_other_calls()
{
    mock().ignoreOtherCalls();
    return DroneCAN_service(can_driver);
}

TEST_GROUP(DroneCAN_service_initialization)
{
    void setup()
    {
        mock().disable();
        custom_can = new CAN_bus_custom();
        mock().enable();
    }
    void teardown()
    {
        teardown_mocks();
        if (custom_can != nullptr)
        {
            delete custom_can;
            custom_can = nullptr;
        }
    }
};

TEST(DroneCAN_service_initialization, system_inits_healthy) {
    DroneCAN_service droneCAN_service = get_DroneCAN_ignoring_other_calls();

    CHECK_TRUE(droneCAN_service.is_healthy());
}

TEST(DroneCAN_service_initialization, on_initialization_DroneCAN_message_sender_is_init)
{
    mock().expectOneCall("DroneCAN_message_sender->constructor")
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();
    DroneCAN_service droneCAN_service(can_driver);
}

extern void handle_canard_reception(CanardInstance* ins, CanardRxTransfer* transfer);
extern bool should_accept_canard_reception(const CanardInstance* ins, 
                                           uint64_t* out_data_type_signature, 
                                           uint16_t data_type_id, CanardTransferType transfer_type,
                                           uint8_t source_node_id);

TEST(DroneCAN_service_initialization,
on_initialization_libcanard_is_initialized_with_reception_functions)
{
    mock().expectOneCall("canard->init_with_reception_handler")
          .withPointerParameter("handle_reception", (void*)handle_canard_reception)
          .withPointerParameter("handle_acceptance", (void*)should_accept_canard_reception);
    mock().ignoreOtherCalls();
    Spied_droneCAN_service spied_dronceCAN_service(can_driver);
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
    
    DroneCAN_service droneCAN_service(can_driver, NODE_ID);
    CHECK_EQUAL(NODE_ID, droneCAN_service.get_node_ID());
}

extern void DUMMY_error_handler(DroneCAN_error error);

TEST(DroneCAN_service_initialization, droneCAN_without_handle_error_function)
{
    mock().ignoreOtherCalls();
    Spied_droneCAN_service droneCAN_service(can_driver);
    
    droneCAN_handle_error_t handle_error = droneCAN_service.get_pointer_to_handle_error_function();
    
    POINTERS_EQUAL(DUMMY_error_handler , handle_error);
}

void handle_error_function(DroneCAN_error error) {}

TEST(DroneCAN_service_initialization, droneCAN_with_handle_error_function)
{
    mock().ignoreOtherCalls();
    Spied_droneCAN_service droneCAN_service(can_driver, DEFAULT_NODE_ID,
                                            handle_error_function);
    droneCAN_handle_error_t handle_error = droneCAN_service.get_pointer_to_handle_error_function();
    
    POINTERS_EQUAL(handle_error_function, handle_error);
}