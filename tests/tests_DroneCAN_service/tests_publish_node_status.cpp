#include <common_to_all_tests.h>
#include "tests_publish_message/common_to_publish_message/common_to_publish_message.h"

TEST_GROUP(DroneCAN_service_publish_node_status)
{
    Canard_message_type_info_t_comparator type_info_comparator;
    Canard_message_data_t_comparator data_comparator;
    void setup()
    {
        mock().installComparator("canard_message_type_info_t", type_info_comparator);
        mock().installComparator("canard_message_data_t", data_comparator);
    }
    void teardown()
    {
        execute_test_teardown();
    }
};

TEST(DroneCAN_service_publish_node_status, send_node_status_message)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    canard_message_type_info_t message_info = {.signature = UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE,
                                               .id = UAVCAN_PROTOCOL_NODESTATUS_ID,
                                               .priority = CANARD_TRANSFER_PRIORITY_HIGH};
    mock().expectOneCall("uavcan_protocol_NodeStatus_encode")
          .ignoreOtherParameters();
    
    mock().expectOneCall("broadcast")
          .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&message_info)
          .ignoreOtherParameters();
    
    mock().ignoreOtherCalls();

    uavcan_protocol_NodeStatus node_status{};
    droneCAN_service.publish_message(node_status);
}

TEST(DroneCAN_service_publish_node_status, publish_regularly_every_configured_seconds)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    mock().expectOneCall("uavcan_protocol_NodeStatus_encode")
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();

    int ACTUAL_TIME = MILLISECONDS_BETWEEN_NODE_STATUS_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

TEST(DroneCAN_service_publish_node_status, is_not_time_to_publish) {
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    mock().expectNoCall("uavcan_protocol_NodeStatus_encode");
    mock().ignoreOtherCalls();

    int ACTUAL_TIME = MILLISECONDS_BETWEEN_NODE_STATUS_PUBLISHES - 1;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

TEST(DroneCAN_service_publish_node_status, if_is_time_to_publish_only_publish_once)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    mock().expectOneCall("uavcan_protocol_NodeStatus_encode")
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();

    int ACTUAL_TIME = MILLISECONDS_BETWEEN_NODE_STATUS_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}