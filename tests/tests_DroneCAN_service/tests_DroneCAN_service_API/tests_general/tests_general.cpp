#include <common_to_DroneCAN_service_tests.h>
#include <DroneCAN_message_sender.h>
#include <auxiliary_functions.h>
#include <CAN_bus_adaptor.h>
#include <uavcan.protocol.param.GetSet_res.h>
#include <uavcan.protocol.param.GetSet_req.h>
#include <uavcan.protocol.GetNodeInfo_req.h>
#include <mocks/HAL_system/HAL_system_singleton.h>

static CAN_driver_interface can_driver;

extern bool is_there_canard_message_to_handle;
typedef struct{
    CanardInstance *instance;
    CanardRxTransfer rx_transfer;
    uint8_t source_node_id;
}canard_reception_t;
extern canard_reception_t canard_reception;

TEST_GROUP(DroneCAN_service_API_general)
{
    CanardCANFrame_comparator can_frame_comparator;
    Uavcan_protocol_param_GetSetResponse_comparator param_GetSetResponse_comparator;
    Uavcan_protocol_getNodeInfoResponse_comparator get_node_info_comparator;
    void setup()
    {
        memset(&canard_reception, 0, sizeof(canard_reception));
        is_there_canard_message_to_handle = false;
        mock().installComparator("CanardCANFrame", can_frame_comparator);
        mock().installComparator("uavcan_protocol_param_GetSetResponse", param_GetSetResponse_comparator);
        mock().installComparator("uavcan_protocol_GetNodeInfoResponse", get_node_info_comparator);
    }
    void teardown()
    {
        mock().removeAllComparatorsAndCopiers();
        mock().checkExpectations();
        mock().clear();
        HAL_system_singleton::delete_instance();
    }
};

TEST(DroneCAN_service_API_general,
WHEN_there_is_can_bus_data_to_read_THEN_sends_it_to_canard)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    const int CAN_BUS_PACKET_SIZE = 8;
    uint8_t can_bus_packet[CAN_BUS_PACKET_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8};
    const long CAN_BUS_PACKET_ID = 99;

    CanardCANFrame can_frame{};
    can_frame.id = CAN_BUS_PACKET_ID;
    memcpy(can_frame.data, can_bus_packet, CAN_BUS_PACKET_SIZE);
    can_frame.data_len = CAN_BUS_PACKET_SIZE;

    mock().expectOneCall("CAN_driver_interface->read_frame")
          .andReturnValue((void*)&can_frame);

    microseconds ACTUAL_TIME = 100;
    mock().expectOneCall("canard->handle_rx_frame")
          .withParameterOfType("CanardCANFrame", "can_frame", (const void*)&can_frame)
          .withUnsignedLongLongIntParameter("timestamp_usec", ACTUAL_TIME);

    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

TEST(DroneCAN_service_API_general,
WHEN_there_is_NO_can_bus_data_to_read_THEN_do_NOT_send_it_to_canard)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    CanardCANFrame can_frame{};
    mock().expectOneCall("CAN_driver_interface->read_frame")
          .andReturnValue((void*)&can_frame);

    mock().expectNoCall("canard->handle_rx_frame");
    droneCAN_service.run_pending_tasks(0);
}

static void handle_error(DroneCAN_error error) {
    mock().actualCall("canard->handle_error")
          .withIntParameter("error", (int)error);
}

IGNORE_TEST(DroneCAN_service_API_general, when_onReceive_from_can_bus_has_error_then_indicates_it) //TODO: add an exception for ERROR codes that are not real errors like: NOT_DESIRED_MESSAGE
{
    mock().disable();
    DroneCAN_service droneCAN_service(can_driver, DEFAULT_NODE_ID, handle_error);
    mock().enable();

    int16_t ERROR_CODE = -40;
    mock().expectOneCall("canard->handle_rx_frame")
          .ignoreOtherParameters()
          .andReturnValue(ERROR_CODE);
    mock().expectOneCall("canard->handle_error")
          .withIntParameter("error", (int)DroneCAN_error::FAIL_ON_RECEPTION);
    mock().ignoreOtherCalls();
    
    microseconds ACTUAL_TIME = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);

    //FIX THIS TEST
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

TEST(DroneCAN_service_API_general, when_uavcan_nodeStatus_is_received_accept_it)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    should_accept_canard_struct accept_struct{};
    accept_struct.data_type_id = UAVCAN_PROTOCOL_NODESTATUS_ID;
    CHECK_TRUE(should_accept_canard_reception(accept_struct.ins, &accept_struct.out_data_type_signature, accept_struct.data_type_id, accept_struct.transfer_type, 0));

    CHECK_EQUAL(UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE, accept_struct.out_data_type_signature);
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
    DroneCAN_service droneCAN_service(can_driver);
    DroneCAN_service_spy spy(&droneCAN_service);
    mock().enable();
    mock().ignoreOtherCalls();

    CanardInstance canard_instance{};
    CanardRxTransfer rx_transfer{};
    rx_transfer.data_type_id = UAVCAN_PROTOCOL_GETNODEINFO_REQUEST_ID;
    rx_transfer.source_node_id = 20;
    handle_canard_reception(&canard_instance, &rx_transfer);
    CHECK_TRUE(is_there_canard_message_to_handle);

    uavcan_protocol_GetNodeInfoResponse node_info_response_to_send{};
    node_info_response_to_send.status = *spy.spy_node_status_struct();
    strcpy((char*)node_info_response_to_send.name.data, DRONECAN_NODE_NAME);
    node_info_response_to_send.name.len = strlen(DRONECAN_NODE_NAME);

    uavcan_protocol_GetNodeInfoResponse internal_node_info = spy.get_nodeInfo();
    
    MEMCMP_EQUAL(&node_info_response_to_send, &internal_node_info,
                 sizeof(uavcan_protocol_GetNodeInfoResponse));

    

    microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);

}

TEST(DroneCAN_service_API_general, set_node_info)
{
    mock().disable();
    DroneCAN_service droneCAN_service(can_driver);
    DroneCAN_service_spy spy(&droneCAN_service);

    char node_name[] = "DroneCAN_DEBUG";
    uavcan_protocol_SoftwareVersion software_version = {.major = 1, .minor = 2};
    uavcan_protocol_HardwareVersion hardware_version = {.major = 3, .minor = 4};
    node_info_data_t node_info_to_set;
    memcpy(&node_info_to_set.software_version, &software_version, sizeof(software_version));
    memcpy(&node_info_to_set.hardware_version, &hardware_version, sizeof(hardware_version));
    node_info_to_set.name.len = strlen(node_name);
    memcpy(&node_info_to_set.name.data, node_name, sizeof(node_name));

    droneCAN_service.set_node_info(node_info_to_set);

    uavcan_protocol_GetNodeInfoResponse internal_node_info = spy.get_nodeInfo();

    CHECK_EQUAL(node_info_to_set.software_version.major, internal_node_info.software_version.major);
    CHECK_EQUAL(node_info_to_set.software_version.minor, internal_node_info.software_version.minor);

    CHECK_EQUAL(node_info_to_set.hardware_version.major, internal_node_info.hardware_version.major);
    CHECK_EQUAL(node_info_to_set.hardware_version.major, internal_node_info.hardware_version.major);
    
    STRCMP_EQUAL((char*)node_info_to_set.name.data, (char*)internal_node_info.name.data);
    CHECK_EQUAL(node_info_to_set.name.len, internal_node_info.name.len);
}

template <typename PARAM_VALUE_TYPE>
void set_parameter_on_droneCAN_service(DroneCAN_service& droneCAN_service,
                                       const char* parameter_name,
                                       PARAM_VALUE_TYPE parameter_value, uavcan_protocol_param_Value_type_t data_type,
                                       bool is_read_only = false);

template <typename PARAM_VALUE_TYPE>
void CHECK_paramGetSet_request_is_decoded(uint16_t requested_parameter_index, const char* parameter_name, PARAM_VALUE_TYPE parameter_value, uavcan_protocol_param_Value_type_t data_type);

TEST(DroneCAN_service_API_general,
handle_paramGetSet_request_asking_for_valid_parameter) {
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    set_parameter_on_droneCAN_service(droneCAN_service, "valid system parameter", 0, UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE);
    set_handle_canard_reception_for_paramGetSet_message();
    
    const uint16_t REQUESTED_PARAMETER_INDEX = 0;
    const uavcan_protocol_param_Value_type_t UNION_TAG_WHEN_IS_ASKED_FOR_PARAMETER = UAVCAN_PROTOCOL_PARAM_VALUE_EMPTY;
    CHECK_paramGetSet_request_is_decoded(REQUESTED_PARAMETER_INDEX, "", 0, UNION_TAG_WHEN_IS_ASKED_FOR_PARAMETER); 
    
    uavcan_parameter_t parameter_returned = droneCAN_service.get_parameter(REQUESTED_PARAMETER_INDEX);
    const uint8_t* pointer_to_source_node_id = &canard_reception.source_node_id;
    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response") 
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)&parameter_returned)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer.source_node_id);
    POINTERS_EQUAL(&canard_reception.source_node_id, pointer_to_source_node_id);

    microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);

    CHECK_FALSE(is_there_canard_message_to_handle);
}

//Parameter is read-write by default
TEST(DroneCAN_service_API_general, handle_paramGetSet_request_asking_for_set_parameter_2_only_with_parameter_name)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    const int OLD_PARAMETER_VALUE = 5;
    const char SYSTEM_PARAMETER_NAME[] = "system parameter";
    set_parameter_on_droneCAN_service(droneCAN_service, SYSTEM_PARAMETER_NAME, OLD_PARAMETER_VALUE,
                                      UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE);
    const char SYSTEM_PARAMETER_TO_CHANGE_NAME[] = "system parameter to change value";
    set_parameter_on_droneCAN_service(droneCAN_service, SYSTEM_PARAMETER_TO_CHANGE_NAME,
                                      OLD_PARAMETER_VALUE,
                                      UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE);
    set_handle_canard_reception_for_paramGetSet_message();

    const int NEW_PARAMETER_VALUE = 10;
    const uint16_t PARAMETER_INDEX_DECODED = 0;
    CHECK_paramGetSet_request_is_decoded(PARAMETER_INDEX_DECODED, SYSTEM_PARAMETER_TO_CHANGE_NAME,
                                         NEW_PARAMETER_VALUE,
                                         UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE);
    
    uavcan_parameter_t parameter_returned = droneCAN_service.get_parameter_by_name(SYSTEM_PARAMETER_TO_CHANGE_NAME);
    parameter_returned.value = package_uavcan_param_value(NEW_PARAMETER_VALUE);

    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response")
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)(uavcan_protocol_param_GetSetResponse*)&parameter_returned)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer.source_node_id);

    mock().ignoreOtherCalls();

    const microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);
}

TEST(DroneCAN_service_API_general,
handle_paramGetSet_request_asking_for_set_float_parameter)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    const float OLD_PARAMETER_VALUE = 1.23;
    const char SYSTEM_PARAMETER_NAME[] = "float system parameter";
    set_parameter_on_droneCAN_service(droneCAN_service, SYSTEM_PARAMETER_NAME,
                                      OLD_PARAMETER_VALUE, UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE);
    set_handle_canard_reception_for_paramGetSet_message();

    const float NEW_PARAMETER_VALUE = 4.56;
    const uint16_t PARAMETER_INDEX_DECODED = 0;
    CHECK_paramGetSet_request_is_decoded(PARAMETER_INDEX_DECODED, SYSTEM_PARAMETER_NAME,
                                         NEW_PARAMETER_VALUE,
                                         UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE);

    uavcan_parameter_t parameter_returned = droneCAN_service.get_parameter_by_name(SYSTEM_PARAMETER_NAME);
    parameter_returned.value = package_uavcan_param_value(NEW_PARAMETER_VALUE);
    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response")
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)(uavcan_protocol_param_GetSetResponse*)&parameter_returned)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer.source_node_id);
    mock().ignoreOtherCalls();
    
    const microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);

}

TEST(DroneCAN_service_API_general,
when_a_read_only_param_is_tried_to_be_changed_THEN_do_not_change_value)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    const float OLD_PARAMETER_VALUE = 1.23;
    const char SYSTEM_PARAMETER_NAME[] = "Read-only parameter";
    const bool READ_ONLY_PARAMETER = true;
    set_parameter_on_droneCAN_service(droneCAN_service, SYSTEM_PARAMETER_NAME, OLD_PARAMETER_VALUE,
                                      UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE, READ_ONLY_PARAMETER);
    set_handle_canard_reception_for_paramGetSet_message();

    const float NEW_PARAMETER_VALUE = 4.56;
    const uint16_t PARAMETER_INDEX_DECODED = 0;
    CHECK_paramGetSet_request_is_decoded(PARAMETER_INDEX_DECODED, SYSTEM_PARAMETER_NAME,
                                         NEW_PARAMETER_VALUE,                                       UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE);

    uavcan_parameter_t parameter_returned = droneCAN_service.get_parameter_by_name(SYSTEM_PARAMETER_NAME);
    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response")
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)(uavcan_protocol_param_GetSetResponse*)&parameter_returned)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer.source_node_id);
    mock().ignoreOtherCalls();
    
    const microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);
}

TEST(DroneCAN_service_API_general,
when_parameter_is_float_and_is_below_min_or_above_max_THEN_do_not_change_value)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    uavcan_parameter_t param{};
    const char SYSTEM_PARAMETER_NAME[] = "Read-only parameter";
    const float OLD_PARAMETER_VALUE = 2.2;
    const float MIN_VALUE = 0.99;
    const float MAX_VALUE = 2.99;
    param.min_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE;
    param.min_value.real_value = MIN_VALUE;
    param.max_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE;
    param.max_value.real_value = MAX_VALUE;
    param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE;
    param.value.real_value = OLD_PARAMETER_VALUE;
    param.name.len = strlen(SYSTEM_PARAMETER_NAME);
    memcpy(&param.name.data, SYSTEM_PARAMETER_NAME, strlen(SYSTEM_PARAMETER_NAME));
    droneCAN_service.add_parameter(param);
    set_handle_canard_reception_for_paramGetSet_message();

    const float NEW_VALUE_BELOW_MIN = MIN_VALUE - 0.01;
    const uint16_t PARAMETER_INDEX_DECODED = 0;
    CHECK_paramGetSet_request_is_decoded(PARAMETER_INDEX_DECODED, SYSTEM_PARAMETER_NAME,
                                         NEW_VALUE_BELOW_MIN,                                       UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE);

    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response")
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)(uavcan_protocol_param_GetSetResponse*)&param)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer.source_node_id);
    mock().ignoreOtherCalls(); 
    const microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);

    set_handle_canard_reception_for_paramGetSet_message();
    const float NEW_VALUE_ABOVE_MAX = MAX_VALUE + 0.01;
    CHECK_paramGetSet_request_is_decoded(PARAMETER_INDEX_DECODED, SYSTEM_PARAMETER_NAME,
                                         NEW_VALUE_ABOVE_MAX,                                       UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE);
    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response")
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)(uavcan_protocol_param_GetSetResponse*)&param)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer.source_node_id);
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);
}

TEST(DroneCAN_service_API_general,
when_parameter_is_integer_and_is_below_min_or_above_max_THEN_do_not_change_value)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    uavcan_parameter_t param{};
    const char SYSTEM_PARAMETER_NAME[] = "Read-only parameter";
    const int OLD_PARAMETER_VALUE = 15;
    const int MIN_VALUE = -5;
    const int MAX_VALUE = 20;
    param.min_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE;
    param.min_value.integer_value = MIN_VALUE;
    param.max_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE;
    param.max_value.integer_value = MAX_VALUE;
    param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
    param.value.integer_value = OLD_PARAMETER_VALUE;
    param.name.len = strlen(SYSTEM_PARAMETER_NAME);
    memcpy(&param.name.data, SYSTEM_PARAMETER_NAME, strlen(SYSTEM_PARAMETER_NAME));
    droneCAN_service.add_parameter(param);
    set_handle_canard_reception_for_paramGetSet_message();

    const int NEW_VALUE_BELOW_MIN = MIN_VALUE - 1;
    const uint16_t PARAMETER_INDEX_DECODED = 0;
    CHECK_paramGetSet_request_is_decoded(PARAMETER_INDEX_DECODED, SYSTEM_PARAMETER_NAME,
                                         NEW_VALUE_BELOW_MIN,                                       UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE);

    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response")
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)(uavcan_protocol_param_GetSetResponse*)&param)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer.source_node_id);
    mock().ignoreOtherCalls(); 
    const microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);

    set_handle_canard_reception_for_paramGetSet_message();
    const int NEW_VALUE_ABOVE_MAX = MAX_VALUE + 1;
    CHECK_paramGetSet_request_is_decoded(PARAMETER_INDEX_DECODED, SYSTEM_PARAMETER_NAME,
                                         NEW_VALUE_ABOVE_MAX,                                       UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE);
    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response")
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)(uavcan_protocol_param_GetSetResponse*)&param)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer.source_node_id);
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);
}

TEST(DroneCAN_service_API_general,
when_parameter_type_received_mismatch_param_type_THEN_do_not_change_value)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    uavcan_parameter_t param{};
    const char SYSTEM_PARAMETER_NAME[] = "Read-only parameter";
    const int OLD_PARAMETER_VALUE = 15;
    const int MIN_VALUE = -5;
    const int MAX_VALUE = 20;
    param.min_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE;
    param.min_value.integer_value = MIN_VALUE;
    param.max_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE;
    param.max_value.integer_value = MAX_VALUE;
    param.value.union_tag = UAVCAN_PROTOCOL_PARAM_VALUE_INTEGER_VALUE;
    param.value.integer_value = OLD_PARAMETER_VALUE;
    param.name.len = strlen(SYSTEM_PARAMETER_NAME);
    memcpy(&param.name.data, SYSTEM_PARAMETER_NAME, strlen(SYSTEM_PARAMETER_NAME));
    droneCAN_service.add_parameter(param);
    set_handle_canard_reception_for_paramGetSet_message();

    const int NEW_VALUE = MIN_VALUE + 1;
    const uint16_t PARAMETER_INDEX_DECODED = 0;
    CHECK_paramGetSet_request_is_decoded(PARAMETER_INDEX_DECODED, SYSTEM_PARAMETER_NAME,
                                         NEW_VALUE, UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE);

    mock().expectOneCall("DroneCAN_message_sender->send_response_message_with_param_response")
          .withParameterOfType("uavcan_protocol_param_GetSetResponse", "param_response", (const void*)(uavcan_protocol_param_GetSetResponse*)&param)
          .withUnsignedIntParameter("destination_node_id", canard_reception.rx_transfer.source_node_id);
    mock().ignoreOtherCalls(); 
    const microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);
}

template <typename PARAM_VALUE_TYPE>
void set_parameter_on_droneCAN_service(DroneCAN_service& droneCAN_service,
                                       const char* parameter_name,
                                       PARAM_VALUE_TYPE parameter_value,
                                       uavcan_protocol_param_Value_type_t data_type,
                                       bool is_read_only)
{
    uavcan_parameter_t system_parameter{};
    system_parameter.is_read_only = is_read_only;
    strcpy((char*)system_parameter.name.data, parameter_name);
    system_parameter.name.len = strlen((char*)system_parameter.name.data);
    system_parameter.value.union_tag = data_type;
    switch (data_type) {
        case UAVCAN_PROTOCOL_PARAM_VALUE_REAL_VALUE:
            system_parameter.value.real_value = parameter_value;
            system_parameter.max_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_REAL_VALUE;
            system_parameter.max_value.real_value = 999999999;
            break;
        default:
            system_parameter.value.integer_value = parameter_value;
            system_parameter.max_value.union_tag = UAVCAN_PROTOCOL_PARAM_NUMERICVALUE_INTEGER_VALUE;
            system_parameter.max_value.integer_value = 99999999;
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
          .withMemoryBufferParameter("transfer", (unsigned char*)&canard_reception.rx_transfer, sizeof(CanardRxTransfer))
          .withOutputParameterReturning("msg", (const void*)&paramGetSet_request, sizeof(paramGetSet_request));
}

//The rx_transfer is passed by copy
TEST(DroneCAN_service_API_general, handle_paramGetSet_request_asking_for_invalid_parameter)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    set_handle_canard_reception_for_paramGetSet_message();

    uavcan_protocol_param_GetSetRequest paramGetSet_request{};
    paramGetSet_request.index = 2;
    mock().expectOneCall("uavcan_protocol_param_GetSetRequest_decode")
          .withMemoryBufferParameter("transfer", (unsigned char*)&canard_reception.rx_transfer, sizeof(CanardRxTransfer))
          .withOutputParameterReturning("msg", (const void*)&paramGetSet_request, sizeof(paramGetSet_request));

    uavcan_parameter_t parameter_returned = droneCAN_service.get_parameter(paramGetSet_request.index);
    STRCMP_EQUAL(NAME_FOR_INVALID_PARAMETER, (char*)parameter_returned.name.data);

    microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);
    CHECK_FALSE(is_there_canard_message_to_handle);
}

static void set_generic_canard_reception(uint16_t UAVCAN_ID, uint8_t sender_node_id);

void set_handle_canard_reception_for_paramGetSet_message()
{
    set_generic_canard_reception(UAVCAN_PROTOCOL_PARAM_GETSET_REQUEST_ID, 20);
}

static void set_multiple_nodeStatus_msg_received(DroneCAN_service& service,
                                                uint8_t initial_ID, uint8_t final_ID);

TEST(DroneCAN_service_API_general,
WHEN_NodeStatus_msg_is_received_from_a_node_with_the_same_ID_THEN_new_device_ID_is_one_plus_the_ID)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    set_multiple_nodeStatus_msg_received(droneCAN_service, DEFAULT_NODE_ID, DEFAULT_NODE_ID);

    CHECK_EQUAL(DEFAULT_NODE_ID + 1, droneCAN_service.get_node_ID());
}



TEST(DroneCAN_service_API_general,
WHEN_NodeStatus_msgs_are_received_from_DEFAULT_ID_to_124_THEN_new_device_ID_is_125)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    set_multiple_nodeStatus_msg_received(droneCAN_service, DEFAULT_NODE_ID, 124);
    CHECK_EQUAL(125, droneCAN_service.get_node_ID());
}

TEST(DroneCAN_service_API_general,
WHEN_NodeStatus_msgs_are_received_exept_for_5_THEN_new_device_ID_is_5)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    set_multiple_nodeStatus_msg_received(droneCAN_service, 1, 4);
    set_multiple_nodeStatus_msg_received(droneCAN_service, 6, 125);

    CHECK_EQUAL(5, droneCAN_service.get_node_ID());
}

//Valid values for ID are 1-125. 1 and 125 included.
//Test invalid values above 125 has not sense because the ID has only 7 bits.
TEST(DroneCAN_service_API_general,
WHEN_NodeStatus_msgs_are_received_from_DEFAULT_ID_to_125_THEN_new_device_ID_is_1)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    set_multiple_nodeStatus_msg_received(droneCAN_service, DEFAULT_NODE_ID, 125);
    CHECK_EQUAL(1, droneCAN_service.get_node_ID());
}

//Valid ID 1-125 with 1 and 125 included.
//This also test the system behaves properly with 0 that is out of range
TEST(DroneCAN_service_API_general,
WHEN_NodeStatus_msgs_are_received_from_0_to_125_included_THEN_new_device_ID_is_the_special_ID_126)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    set_multiple_nodeStatus_msg_received(droneCAN_service, 0, 125);
    CHECK_EQUAL(126, droneCAN_service.get_node_ID());
}

TEST(DroneCAN_service_API_general,
GIVEN_all_IDs_are_taken_by_nodes_WHEN_passed_1_sec_and_all_IDs_are_free_THEN_new_device_ID_is_1)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();
    set_multiple_nodeStatus_msg_received(droneCAN_service, 0, 125);

    mock().expectOneCall("CAN_driver_interface->read_frame")
          .ignoreOtherParameters();
    droneCAN_service.run_pending_tasks(1e6);
    CHECK_EQUAL(1, droneCAN_service.get_node_ID());
}

static void set_multiple_nodeStatus_msg_received(DroneCAN_service& service,
                                                uint8_t initial_ID, uint8_t final_ID)
{
    static CanardCANFrame frame{};
    for (int sender_id = initial_ID; sender_id <= final_ID; sender_id++)
    {

        frame.id = sender_id;
        mock().expectOneCall("CAN_driver_interface->read_frame")
              .andReturnValue((void*)&frame);

        microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
        service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);
    }
}

static void set_generic_canard_reception(uint16_t UAVCAN_ID, uint8_t sender_node_id)
{
    static CanardInstance canard_instance{};
    static CanardRxTransfer rx_transfer{};
    rx_transfer.data_type_id = UAVCAN_ID;
    rx_transfer.source_node_id = sender_node_id;
    handle_canard_reception(&canard_instance, &rx_transfer);
    CHECK_TRUE(is_there_canard_message_to_handle);
}

TEST(DroneCAN_service_API_general,
WHEN_run_pending_tasks_THEN_set_node_id_based_on_stored_ID)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    mock().expectOneCall("canard->set_node_ID")
        .withUnsignedIntParameter("self_node_id", DEFAULT_NODE_ID);
    
    microseconds ACTUAL_TIME_DOES_NOT_MATTER = 0;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME_DOES_NOT_MATTER);
}

#define MS_TO_CONSIDER_CAN_BUS_INACTIVE 10e3
#define MS_TO_USECS(x) 1000*x

TEST(DroneCAN_service_API_general,
GIVEN_no_received_data_from_can_bus_WHEN_time_for_inactive_is_reached_THEN_consider_CAN_inactive)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    // device->set_millisecs_since_init(MS_TO_CONSIDER_CAN_BUS_INACTIVE);
    
    uint32_t ms_since_init = MS_TO_CONSIDER_CAN_BUS_INACTIVE;
    droneCAN_service.run_pending_tasks(MS_TO_USECS(ms_since_init));
    CHECK_TRUE(droneCAN_service.is_CAN_bus_inactive(MS_TO_CONSIDER_CAN_BUS_INACTIVE,
                                                    ms_since_init));
}

TEST(DroneCAN_service_API_general,
GIVEN_no_initialized_system_WHEN_initialize_system_THEN_consider_CAN_active)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    uint32_t milliseconds_since_init = 0;
    CHECK_FALSE(droneCAN_service.is_CAN_bus_inactive(MS_TO_CONSIDER_CAN_BUS_INACTIVE,
                                                     milliseconds_since_init));
}

TEST(DroneCAN_service_API_general,
GIVEN_received_data_WHEN_time_for_inactive_is_reached_THEN_is_can_bus_inactive_returns_true)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    mock().ignoreOtherCalls();

    CanardCANFrame recieved_frame_different_from_empty{.id = 99, .data = {1,2,3}, .data_len = 3};
    mock().expectOneCall("CAN_driver_interface->read_frame")
          .andReturnValue((void*)&recieved_frame_different_from_empty);
    
    uint32_t ms_since_init = MS_TO_CONSIDER_CAN_BUS_INACTIVE - 1;
    droneCAN_service.run_pending_tasks(MS_TO_USECS(ms_since_init));

    ms_since_init = MS_TO_CONSIDER_CAN_BUS_INACTIVE;
    CHECK_FALSE(droneCAN_service.is_CAN_bus_inactive(MS_TO_CONSIDER_CAN_BUS_INACTIVE,
                                                     ms_since_init));
}