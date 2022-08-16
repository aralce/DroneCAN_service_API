#include <common_to_DroneCAN_service_tests.h>
#include <DroneCAN_message_sender.h>
#include <CAN_bus_adaptor.h>
#include <uavcan.protocol.param.GetSet_res.h>
#include <uavcan.protocol.param.GetSet_req.h>

extern bool is_there_canard_message_to_handle;
typedef struct{
    CanardInstance *instance;
    CanardRxTransfer *rx_transfer;
}canard_reception_t;
extern canard_reception_t canard_reception;

TEST_GROUP(DroneCAN_service_API_general)
{
    CanardCANFrame_comparator can_frame_comparator;
    Uavcan_protocol_param_GetSetResponse_comparator param_GetSetResponse_comparator;
    void setup()
    {
        memset(&canard_reception, 0, sizeof(canard_reception));
        is_there_canard_message_to_handle = false;
        mock().installComparator("CanardCANFrame", can_frame_comparator);
        mock().installComparator("uavcan_protocol_param_GetSetResponse", param_GetSetResponse_comparator);
    }
    void teardown()
    {
        mock().removeAllComparatorsAndCopiers();
        mock().checkExpectations();
        mock().clear();
    }
};

extern void onReceive_on_can_bus(int packet_size);
extern bool is_can_data_to_read;
TEST(DroneCAN_service_API_general, onReceive_from_can_bus_read_data_and_sends_it_to_canard)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    
    const int CAN_BUS_PACKET_SIZE = 8;
    uint8_t can_bus_packet[CAN_BUS_PACKET_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};
    
    is_can_data_to_read = true;

    const long CAN_BUS_PACKET_ID = 99;
    mock().expectOneCall("CAN_bus_adaptor->get_packet_id")
          .andReturnValue(CAN_BUS_PACKET_ID);

    for (int byte = 0; byte < CAN_BUS_PACKET_SIZE; ++byte) {
        mock().expectOneCall("CAN_bus_adaptor->read")
              .andReturnValue(can_bus_packet[byte]);
    }

    milliseconds ACTUAL_TIME = 100;
    CanardCANFrame can_frame{};
    can_frame.id = CAN_BUS_PACKET_ID;
    memcpy(can_frame.data, can_bus_packet, CAN_BUS_PACKET_SIZE);
    can_frame.data_len = CAN_BUS_PACKET_SIZE;
    mock().expectOneCall("canard->handle_rx_frame")
          .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&can_frame)
          .withUnsignedLongLongIntParameter("timestamp_usec", 1000*ACTUAL_TIME);

    droneCAN_service.run_pending_tasks(ACTUAL_TIME);

    CHECK_FALSE(is_can_data_to_read);
}

static void handle_error(DroneCAN_error error) {
    mock().actualCall("canard->handle_error")
          .withIntParameter("error", (int)error);
}

TEST(DroneCAN_service_API_general, when_onReceive_from_can_bus_has_error_then_indicates_it)
{
    mock().disable();
    DroneCAN_service droneCAN_service(DEFAULT_NODE_ID, handle_error);
    mock().enable();

    is_can_data_to_read = true;
    int16_t ERROR_CODE = -40;
    mock().expectOneCall("canard->handle_rx_frame")
          .ignoreOtherParameters()
          .andReturnValue(ERROR_CODE);
    mock().expectOneCall("canard->handle_error")
          .withIntParameter("error", (int)DroneCAN_error::FAIL_ON_RECEPTION);
    mock().ignoreOtherCalls();
    
    milliseconds ACTUAL_TIME = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
    
    CHECK_FALSE(is_can_data_to_read);
}

extern bool should_accept_canard_reception(const CanardInstance* ins, uint64_t* out_data_type_signature, uint16_t data_type_id, CanardTransferType transfer_type, uint8_t source_node_id);
struct should_accept_canard_struct {
    CanardInstance* ins;
    uint64_t out_data_type_signature;
    uint16_t data_type_id;
    CanardTransferType transfer_type;
    uint8_t source_node_id;
};

TEST(DroneCAN_service_API_general, when_uavcan_protocol_paramGetSet_request_is_received_accept_it)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    should_accept_canard_struct accept_struct{};
    accept_struct.data_type_id = UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_ID;
    CHECK_TRUE(should_accept_canard_reception(accept_struct.ins, &accept_struct.out_data_type_signature, accept_struct.data_type_id, accept_struct.transfer_type, 0));

    CHECK_EQUAL(UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_SIGNATURE, accept_struct.out_data_type_signature);
}

TEST(DroneCAN_service_API_general, when_uavcan_nodeStatus_is_received_does_not_accept_it)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    should_accept_canard_struct accept_struct{};
    accept_struct.data_type_id = UAVCAN_PROTOCOL_NODESTATUS_ID;
    CHECK_FALSE(should_accept_canard_reception(accept_struct.ins, &accept_struct.out_data_type_signature, accept_struct.data_type_id, accept_struct.transfer_type, 0));
}

extern void handle_canard_reception(CanardInstance* ins, CanardRxTransfer* transfer);

TEST(DroneCAN_service_API_general, received_message_is_handled_on_canard_reception_function)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    CHECK_FALSE(is_there_canard_message_to_handle);

    CanardInstance canard_instance;
    CanardRxTransfer rx_transfer;
    handle_canard_reception(&canard_instance, &rx_transfer);

    CHECK_TRUE(is_there_canard_message_to_handle);
    POINTERS_EQUAL(&canard_instance, canard_reception.instance);
}

TEST(DroneCAN_service_API_general, handle_paramGetSet_request_asking_for_valid_parameter) {
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    uavcan_parameter system_parameter{};
    strcpy((char*)system_parameter.name.data,"system parameter");
    system_parameter.name.len = strlen((char*)system_parameter.name.data);
    droneCAN_service.add_parameter(system_parameter);

    CanardInstance canard_instance;
    CanardRxTransfer rx_transfer;
    rx_transfer.data_type_id = UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_ID;
    rx_transfer.source_node_id = 20;
    handle_canard_reception(&canard_instance, &rx_transfer);

    CHECK_TRUE(is_there_canard_message_to_handle);
    
    uavcan_protocol_param_GetSetRequest paramGetSet_request{};
    paramGetSet_request.name.len = 0;
    const uint16_t REQUESTED_PARAMETER_INDEX = 0;
    paramGetSet_request.index = REQUESTED_PARAMETER_INDEX;
    mock().expectOneCall("uavcan_protocol_param_GetSetRequest_decode")
          .withPointerParameter("transfer", (void*)canard_reception.rx_transfer)
          .withOutputParameterReturning("msg", (const void*)&paramGetSet_request, sizeof(paramGetSet_request));
    
    uavcan_parameter parameter_returned = droneCAN_service.get_parameter(REQUESTED_PARAMETER_INDEX);
    mock().expectOneCall("DroneCAN_message_sender->send_response_message") 
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)&parameter_returned)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer->source_node_id);

    milliseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);

    CHECK_FALSE(is_there_canard_message_to_handle);
}

TEST(DroneCAN_service_API_general)