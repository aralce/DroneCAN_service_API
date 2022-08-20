#include <common_to_all_tests.h>
#include <DSDL_to_canard_DTO.h>

TEST_GROUP(DSDL_to_canard_DTO)
{
    void teardown()
    {
        mock().checkExpectations();
        mock().clear();
    }
};

static uint8_t buffer[8] = {1, 2, 3, 4, 5, 6, 7, 8};
static const uint32_t RETURNING_VALUE = sizeof(buffer);

static void CHECK_message_type_and_message_data(DSDL_to_canard_DTO& data_transfer_object, canard_message_type_info_t& message_type_info, uint8_t* buffer, uint8_t buffer_len);

TEST(DSDL_to_canard_DTO, convert_uavcan_equipment_power_BatteryInfo)
{
    uavcan_equipment_power_BatteryInfo battery_info;
    
    mock().expectOneCall("uavcan_equipment_power_BatteryInfo_encode")
          .withParameter("msg", (void*)&battery_info)
          .withOutputParameterReturning("buffer", (const void*)buffer, sizeof(buffer))
          .andReturnValue(RETURNING_VALUE);
    DSDL_to_canard_DTO data_transfer_object(battery_info);

    canard_message_type_info_t message_type_info = {.signature = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_SIGNATURE,
                                                    .id = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_ID,
                                                    .priority = CANARD_TRANSFER_PRIORITY_MEDIUM};
    CHECK_message_type_and_message_data(data_transfer_object, message_type_info, buffer, sizeof(buffer));
}

TEST(DSDL_to_canard_DTO, convert_uavcan_protocol_NodeStatus)
{
    uavcan_protocol_NodeStatus node_status;
           
    mock().expectOneCall("uavcan_protocol_NodeStatus_encode")
          .withParameter("msg", (const void*)&node_status)
          .withOutputParameterReturning("buffer", (const void*)buffer, sizeof(buffer))
          .andReturnValue(RETURNING_VALUE);
    DSDL_to_canard_DTO data_transfer_object(node_status);

    canard_message_type_info_t message_type_info = {.signature = UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE,
                                                    .id = UAVCAN_PROTOCOL_NODESTATUS_ID,
                                                    .priority = CANARD_TRANSFER_PRIORITY_HIGH};
    CHECK_message_type_and_message_data(data_transfer_object, message_type_info, buffer, sizeof(buffer));
}

TEST(DSDL_to_canard_DTO, convert_uavcan_protocol_paramGetSet_response)
{
    uavcan_protocol_param_GetSetResponse param_response;

    mock().expectOneCall("uavcan_protocol_param_GetSetResponse_encode")
          .withParameter("msg", (const void*)&param_response)
          .withOutputParameterReturning("buffer", (const void*)buffer,sizeof(buffer))
          .andReturnValue(RETURNING_VALUE);

    DSDL_to_canard_DTO data_transfer_object(param_response);

    canard_message_type_info_t message_type_info = {.signature = UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_SIGNATURE,
                                                    .id = UAVCAN_PROTOCOL_PARAM_GETSET_RESPONSE_ID,
                                                    .priority = CANARD_TRANSFER_PRIORITY_MEDIUM};
    CHECK_message_type_and_message_data(data_transfer_object, message_type_info, buffer, sizeof(buffer));
}

TEST(DSDL_to_canard_DTO, convert_uavcan_protocol_getNo)
{
    uavcan_protocol_GetNodeInfoResponse get_node_info_request;
}

static void CHECK_message_type_and_message_data(DSDL_to_canard_DTO& data_transfer_object, canard_message_type_info_t& message_type_info, uint8_t* buffer, uint8_t buffer_len)
{
    canard_message_type_info_t message_type_gotten = data_transfer_object.get_type_info();
    CHECK_EQUAL(message_type_info.signature, message_type_gotten.signature);
    CHECK_EQUAL(message_type_info.id, message_type_gotten.id);
    CHECK_EQUAL(message_type_info.priority, message_type_gotten.priority);

    canard_message_data_t message_data = {.ptr = (void*)buffer, .length = sizeof(buffer)};
    canard_message_data_t message_data_gotten = data_transfer_object.get_data();
    MEMCMP_EQUAL(message_data.ptr, message_data_gotten.ptr, message_data.length);
    CHECK_EQUAL(message_data.length, message_data_gotten.length);
}