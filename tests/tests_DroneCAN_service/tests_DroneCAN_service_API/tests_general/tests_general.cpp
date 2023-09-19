#include <common_to_DroneCAN_service_tests.h>
#include <DroneCAN_message_sender.h>
#include <auxiliary_functions.h>
#include <CAN_bus_adaptor.h>
#include <uavcan.protocol.param.GetSet_res.h>
#include <uavcan.protocol.param.GetSet_req.h>
#include <uavcan.protocol.GetNodeInfo_req.h>
#include <mocks/HAL_system/HAL_system_singleton.h>

extern bool is_there_canard_message_to_handle;
typedef struct{
    CanardInstance *instance;
    CanardRxTransfer *rx_transfer;
    uint8_t source_node_id;
}canard_reception_t;
extern canard_reception_t canard_reception;

extern bool is_can_data_to_read;
extern uint32_t ms_since_last_rx;
HAL_system_api* device = nullptr;

TEST_GROUP(DroneCAN_service_API_general)
{
    CanardCANFrame_comparator can_frame_comparator;
    Uavcan_protocol_param_GetSetResponse_comparator param_GetSetResponse_comparator;
    Uavcan_protocol_getNodeInfoResponse_comparator get_node_info_comparator;
    void setup()
    {
        memset(&canard_reception, 0, sizeof(canard_reception));
        is_there_canard_message_to_handle = false;
        is_can_data_to_read = false;
        ms_since_last_rx = 0;
        mock().installComparator("CanardCANFrame", can_frame_comparator);
        mock().installComparator("uavcan_protocol_param_GetSetResponse", param_GetSetResponse_comparator);
        mock().installComparator("uavcan_protocol_GetNodeInfoResponse", get_node_info_comparator);
        device = HAL_system_singleton::get_HAL_system_instance();
    }
    void teardown()
    {
        mock().removeAllComparatorsAndCopiers();
        mock().checkExpectations();
        mock().clear();
        HAL_system_singleton::delete_instance();
    }
};

extern void onReceive_on_can_bus(int packet_size);
extern bool is_can_data_to_read;
TEST(DroneCAN_service_API_general, onReceive_from_can_bus_read_data_and_sends_it_to_canard)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    
    const int CAN_BUS_PACKET_SIZE = 8;
    uint8_t can_bus_packet[CAN_BUS_PACKET_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};
    const long CAN_BUS_PACKET_ID = 99;
    is_can_data_to_read = true;

    CanardCANFrame can_frame{};
    can_frame.id = CAN_BUS_PACKET_ID;
    memcpy(can_frame.data, can_bus_packet, CAN_BUS_PACKET_SIZE);
    can_frame.data_len = CAN_BUS_PACKET_SIZE;

    mock().expectOneCall("CAN_bus_adaptor->read")
          .andReturnValue((void*)&can_frame);

    microseconds ACTUAL_TIME = 100;
    mock().expectOneCall("canard->handle_rx_frame")
          .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&can_frame)
          .withUnsignedLongLongIntParameter("timestamp_usec", ACTUAL_TIME);

    droneCAN_service.run_pending_tasks(ACTUAL_TIME);

    CHECK_FALSE(is_can_data_to_read);
}

static void handle_error(DroneCAN_error error) {
    mock().actualCall("canard->handle_error")
          .withIntParameter("error", (int)error);
}

IGNORE_TEST(DroneCAN_service_API_general, when_onReceive_from_can_bus_has_error_then_indicates_it) //TODO: add an exception for ERROR codes that are not real errors like: NOT_DESIRED_MESSAGE
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
    
    microseconds ACTUAL_TIME = 0;
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

TEST(DroneCAN_service_API_general, when_uavcan_protocol_getNodeInfo_is_received_accept_it)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    should_accept_canard_struct accept_struct{};
    accept_struct.data_type_id = UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_ID;
    CHECK_TRUE(should_accept_canard_reception(accept_struct.ins, &accept_struct.out_data_type_signature, accept_struct.data_type_id, accept_struct.transfer_type, 0));

    CHECK_EQUAL(UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_SIGNATURE, accept_struct.out_data_type_signature);
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

void set_handle_canard_reception_for_paramGetSet_message();


TEST(DroneCAN_service_API_general, handle_getNodeInfo_request)
{
    mock().disable();
    Spied_droneCAN_service spied_droneCAN_service;
    mock().enable();

    CanardInstance canard_instance{};
    CanardRxTransfer rx_transfer{};
    rx_transfer.data_type_id = UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_ID;
    rx_transfer.source_node_id = 20;
    handle_canard_reception(&canard_instance, &rx_transfer);
    CHECK_TRUE(is_there_canard_message_to_handle);

    uavcan_protocol_GetNodeInfoResponse get_node_info_response{};
    get_node_info_response.status = *spied_droneCAN_service.spy_node_status_struct();
    strcpy((char*)get_node_info_response.name.data, DRONECAN_NODE_NAME);
    get_node_info_response.name.len = strlen(DRONECAN_NODE_NAME);

    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_get_node_info_response")
          .withParameterOfType("uavcan_protocol_GetNodeInfoResponse", "get_node_info_response", (const void *)&get_node_info_response)
          .withUnsignedIntParameter("destination_node_id", canard_reception.source_node_id);
    
    microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    spied_droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);

}

template <typename PARAM_VALUE_TYPE>
void set_parameter_on_droneCAN_service(DroneCAN_service& droneCAN_service, const char* parameter_name, PARAM_VALUE_TYPE parameter_value, uavcan_protocol_param_Value_type_t data_type);

template <typename PARAM_VALUE_TYPE>
void CHECK_paramGetSet_request_is_decoded(uint16_t requested_parameter_index, const char* parameter_name, PARAM_VALUE_TYPE parameter_value, uavcan_protocol_param_Value_type_t data_type);

TEST(DroneCAN_service_API_general, handle_paramGetSet_request_asking_for_valid_parameter) {
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    set_parameter_on_droneCAN_service(droneCAN_service, "valid system parameter", 0, UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE);
    set_handle_canard_reception_for_paramGetSet_message();
    
    const uint16_t REQUESTED_PARAMETER_INDEX = 0;
    const uavcan_protocol_param_Value_type_t UNION_TAG_WHEN_IS_ASKED_FOR_PARAMETER = UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY;
    CHECK_paramGetSet_request_is_decoded(REQUESTED_PARAMETER_INDEX, "", 0, UNION_TAG_WHEN_IS_ASKED_FOR_PARAMETER); 
    
    uavcan_parameter parameter_returned = droneCAN_service.get_parameter(REQUESTED_PARAMETER_INDEX);
    const uint8_t* pointer_to_source_node_id = &canard_reception.source_node_id;
    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response") 
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)&parameter_returned)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer->source_node_id);
    POINTERS_EQUAL(&canard_reception.source_node_id, pointer_to_source_node_id);

    microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);

    CHECK_FALSE(is_there_canard_message_to_handle);
}

//set_parameter is not implemented //we want read only parameters
IGNORE_TEST(DroneCAN_service_API_general, handle_paramGetSet_request_asking_for_set_parameter_2_only_with_parameter_name)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    const int OLD_PARAMETER_VALUE = 5;
    const char SYSTEM_PARAMETER_NAME[] = "system parameter";
    set_parameter_on_droneCAN_service(droneCAN_service, SYSTEM_PARAMETER_NAME, OLD_PARAMETER_VALUE, UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE);
    const char SYSTEM_PARAMETER_TO_CHANGE_NAME[] = "system parameter to change value";
    set_parameter_on_droneCAN_service(droneCAN_service, SYSTEM_PARAMETER_TO_CHANGE_NAME, OLD_PARAMETER_VALUE, UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE);
    set_handle_canard_reception_for_paramGetSet_message();

    const int NEW_PARAMETER_VALUE = 10;
    const uint16_t PARAMETER_INDEX_DECODED = 0;
    CHECK_paramGetSet_request_is_decoded(PARAMETER_INDEX_DECODED, SYSTEM_PARAMETER_TO_CHANGE_NAME, NEW_PARAMETER_VALUE, UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE);
    
    uavcan_parameter parameter_returned = droneCAN_service.get_parameter_by_name(SYSTEM_PARAMETER_TO_CHANGE_NAME);
    parameter_returned.value = package_uavcan_param_value(NEW_PARAMETER_VALUE);
    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response")
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)&parameter_returned)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer->source_node_id);

    const microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);
}

//set_parameter is not implemented //we want read only parameters
IGNORE_TEST(DroneCAN_service_API_general, handle_paramGetSet_request_asking_for_set_float_parameter)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    const float OLD_PARAMETER_VALUE = 1.23;
    const char SYSTEM_PARAMETER_NAME[] = "float system parameter";
    set_parameter_on_droneCAN_service(droneCAN_service, SYSTEM_PARAMETER_NAME, OLD_PARAMETER_VALUE, UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE);
    set_handle_canard_reception_for_paramGetSet_message();

    const float NEW_PARAMETER_VALUE = 4.56;
    const uint16_t PARAMETER_INDEX_DECODED = 0;
    CHECK_paramGetSet_request_is_decoded(PARAMETER_INDEX_DECODED, SYSTEM_PARAMETER_NAME, NEW_PARAMETER_VALUE, UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE);

    uavcan_parameter parameter_returned = droneCAN_service.get_parameter_by_name(SYSTEM_PARAMETER_NAME);
    parameter_returned.value = package_uavcan_param_value(NEW_PARAMETER_VALUE);
    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response")
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)&parameter_returned)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer->source_node_id);
    
    const microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);
}

template <typename PARAM_VALUE_TYPE>
void set_parameter_on_droneCAN_service(DroneCAN_service& droneCAN_service, const char* parameter_name, PARAM_VALUE_TYPE parameter_value, uavcan_protocol_param_Value_type_t data_type)
{
    uavcan_parameter system_parameter{};
    strcpy((char*)system_parameter.name.data, parameter_name);
    system_parameter.name.len = strlen((char*)system_parameter.name.data);
    system_parameter.value.union_tag = data_type;
    switch (data_type) {
        case UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE:
            system_parameter.value.real_value = parameter_value;
            break;
        default:
            system_parameter.value.integer_value = parameter_value;
            break;
    }
    droneCAN_service.add_parameter(system_parameter);
}

template <typename PARAM_VALUE_TYPE>
void CHECK_paramGetSet_request_is_decoded(uint16_t requested_parameter_index, const char* parameter_name, PARAM_VALUE_TYPE parameter_value, uavcan_protocol_param_Value_type_t data_type)
{
    static uavcan_protocol_param_GetSetRequest paramGetSet_request{};
    paramGetSet_request.index = requested_parameter_index;
    strcpy((char*)paramGetSet_request.name.data, parameter_name);
    paramGetSet_request.name.len = strlen(parameter_name);
    paramGetSet_request.value.union_tag = data_type;
    switch (data_type) {
        case UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE:
            paramGetSet_request.value.real_value = parameter_value;
            break;
        default:
            paramGetSet_request.value.integer_value = parameter_value;
            break;
    }
    mock().expectOneCall("uavcan_protocol_param_GetSetRequest_decode")
          .withPointerParameter("transfer", (void*)canard_reception.rx_transfer)
          .withOutputParameterReturning("msg", (const void*)&paramGetSet_request, sizeof(paramGetSet_request));
}

TEST(DroneCAN_service_API_general, handle_paramGetSet_request_asking_for_invalid_parameter)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    set_handle_canard_reception_for_paramGetSet_message();

    uavcan_protocol_param_GetSetRequest paramGetSet_request{};
    paramGetSet_request.index = 2;
    mock().expectOneCall("uavcan_protocol_param_GetSetRequest_decode")
          .withPointerParameter("transfer", (void*)canard_reception.rx_transfer)
          .withOutputParameterReturning("msg", (const void*)&paramGetSet_request, sizeof(paramGetSet_request));

    uavcan_parameter parameter_returned = droneCAN_service.get_parameter(paramGetSet_request.index);
    STRCMP_EQUAL(NAME_FOR_INVALID_PARAMETER, (char*)parameter_returned.name.data);

    microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);

    CHECK_FALSE(is_there_canard_message_to_handle);
}

void set_handle_canard_reception_for_paramGetSet_message()
{
    static CanardInstance canard_instance{};
    static CanardRxTransfer rx_transfer{};
    rx_transfer.data_type_id = UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_ID;
    rx_transfer.source_node_id = 20;
    handle_canard_reception(&canard_instance, &rx_transfer);
    CHECK_TRUE(is_there_canard_message_to_handle);
}

#define MS_TO_CONSIDER_CAN_BUS_INACTIVE 10e3

TEST(DroneCAN_service_API_general, GIVEN_no_received_data_from_can_bus_WHEN_ms_to_consider_inactive_can_bus_is_reached_THEN_is_can_bus_inactive_returns_true)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    device->set_millisecs_since_init(MS_TO_CONSIDER_CAN_BUS_INACTIVE);

    droneCAN_service.run_pending_tasks(device->millisecs_since_init());
    CHECK_TRUE(droneCAN_service.is_CAN_bus_inactive(MS_TO_CONSIDER_CAN_BUS_INACTIVE));
}

TEST(DroneCAN_service_API_general, GIVEN_no_initialized_system_WHEN_initialize_system_THEN_is_can_bus_inactive_returns_false)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    CHECK_FALSE(droneCAN_service.is_CAN_bus_inactive(MS_TO_CONSIDER_CAN_BUS_INACTIVE));
}

extern void onReceive_on_can_bus(int packet_size);

TEST(DroneCAN_service_API_general, GIVEN_received_data_on_can_bus_WHEN_ms_to_consider_inactive_can_bus_is_reached_without_rx_on_can_THEN_is_can_bus_inactive_returns_true)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    device->set_millisecs_since_init(MS_TO_CONSIDER_CAN_BUS_INACTIVE -1);

    onReceive_on_can_bus(0);
    mock().ignoreOtherCalls();

    device->set_millisecs_since_init(MS_TO_CONSIDER_CAN_BUS_INACTIVE);
    droneCAN_service.run_pending_tasks(device->millisecs_since_init());

    CHECK_FALSE(droneCAN_service.is_CAN_bus_inactive(MS_TO_CONSIDER_CAN_BUS_INACTIVE));

    device->set_millisecs_since_init(2*MS_TO_CONSIDER_CAN_BUS_INACTIVE);
    droneCAN_service.run_pending_tasks(device->millisecs_since_init());

    CHECK_TRUE(droneCAN_service.is_CAN_bus_inactive(MS_TO_CONSIDER_CAN_BUS_INACTIVE));
}