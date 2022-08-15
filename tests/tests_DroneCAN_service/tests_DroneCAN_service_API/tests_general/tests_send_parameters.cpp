#include <common_to_all_tests.h>
#include <common_to_DroneCAN_service_tests.h>
#include <auxiliary_functions.h>

TEST_GROUP(DroneCAN_service_paramGetSet_publish)
{
    Canard_message_type_info_t_comparator type_info_comparator;
    Uavcan_protocol_param_GetSetResponse_comparator param_GetSetResponse_comparator;
    Canard_message_data_t_comparator data_comparator;
    void setup()
    {
        mock().installComparator("canard_message_type_info_t", type_info_comparator);
        mock().installComparator("uavcan_protocol_param_GetSetResponse", param_GetSetResponse_comparator);
        mock().installComparator("canard_message_data_t", data_comparator);
    }
    void teardown()
    {
        mock().removeAllComparatorsAndCopiers();
    }
};

TEST(DroneCAN_service_paramGetSet_publish, init)
{
    
}

// IGNORE_TEST(DroneCAN_service_paramGetSet_publish, encode_parameter_to_send)
// {
//     mock().ignoreOtherCalls();
//     Spied_droneCAN_service spied_droneCAN_service;

//     uavcan_parameter param_to_send;
//     strcpy((char *)param_to_send.name.data, "param_to_send");
//     param_to_send.name.len = strlen((char *)param_to_send.name.data);
//     const int PARAM_VALUE = 20;
//     param_to_send.value = package_uavcan_param_value(PARAM_VALUE);
//     spied_droneCAN_service.add_parameter(param_to_send);

//     const unsigned int BYTES_ENCODED = 50;
//     mock().expectOneCall("uavcan_protocol_param_GetSetResponse_encode")
//           .withParameterOfType("uavcan_protocol_param_GetSetResponse", "msg", (const void *)&param_to_send)
//           .andReturnValue(BYTES_ENCODED);

//     const uint8_t INDEX_OF_PARAMETER_TO_SEND = 0;
//     spied_droneCAN_service.respond_with_parameter_data(INDEX_OF_PARAMETER_TO_SEND);
// }

// TEST(DroneCAN_service_paramGetSet_publish, send_parameter_with_canard_after_encode_it)
// {
//     mock().ignoreOtherCalls();
//     Spied_droneCAN_service spied_droneCAN_service;

//     uavcan_parameter parameter_to_send;
//     spied_droneCAN_service.add_parameter(parameter_to_send);

//     mock().expectOneCall("uavcan_protocol_param_GetSetResponse_encode")
//           .ignoreOtherParameters();

//     const canard_message_type_info_t message_info = {.signature = UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_SIGNATURE,
//                                                      .id = UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_ID,
//                                                      .priority = CANARD_TRANSFER_PRIORITY_MEDIUM};
//     mock().expectOneCall("canard->send_response")
//           .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&message_info)
//           .ignoreOtherParameters();

//     const uint8_t INDEX_OF_PARAMETER_TO_SEND = 0;
//     spied_droneCAN_service.respond_with_parameter_data(INDEX_OF_PARAMETER_TO_SEND);
// }

// TEST(DroneCAN_service_paramGetSet_publish, send_parameter_through_CAN_BUS)
// {
//     mock().ignoreOtherCalls();
//     Spied_droneCAN_service spied_droneCAN_service;

//     uavcan_parameter parameter_to_send;
//     spied_droneCAN_service.add_parameter(parameter_to_send);

//     mock().expectOneCall("uavcan_protocol_param_GetSetResponse_encode")
//           .ignoreOtherParameters();
//     mock().expectOneCall("canard->send_response")
//           .ignoreOtherParameters();
    
//     mock().expectOneCall("canard->is_txQueue_empty")
//           .andReturnValue(false);

//     mock().expectOneCall("canard->peekTxQueue");

//     mock().expectOneCall("canard->popTxQueue");

//     mock().expectOneCall("canard->is_txQueue_empty")
//           .andReturnValue(true);

//     const uint8_t INDEX_OF_PARAMETER_TO_SEND = 0;
//     spied_droneCAN_service.respond_with_parameter_data(INDEX_OF_PARAMETER_TO_SEND);
// }