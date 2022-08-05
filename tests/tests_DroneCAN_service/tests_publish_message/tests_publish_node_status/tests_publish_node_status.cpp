#include <common_to_all_tests.h>
#include "../common_to_publish_message/common_to_publish_message.h"

void expect_one_publish_of_node_status()
{
    mock().expectOneCall("uavcan_protocol_NodeStatus_encode")
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();
}

TEST_GROUP(DroneCAN_service_publish_node_status)
{
    Canard_message_type_info_t_comparator type_info_comparator;
    Canard_message_data_t_comparator data_comparator;
    Uavcan_protocol_NodeStatus_comparator node_status_comparator;
    void setup()
    {
        mock().installComparator("canard_message_type_info_t", type_info_comparator);
        mock().installComparator("canard_message_data_t", data_comparator);
        mock().installComparator("uavcan_protocol_NodeStatus", node_status_comparator);
    }
    void teardown()
    {
        execute_test_teardown();
    }
};

TEST(DroneCAN_service_publish_node_status, send_node_status_message)
{
    mock().disable();
    Spied_droneCAN_service spied_droneCAN_service;
    mock().enable();
    canard_message_type_info_t message_info = {.signature = UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE,
                                               .id = UAVCAN_PROTOCOL_NODESTATUS_ID,
                                               .priority = CANARD_TRANSFER_PRIORITY_HIGH};
    
    expect_one_publish_of_node_status();   
    
    mock().expectOneCall("canard->broadcast")
          .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&message_info)
          .ignoreOtherParameters();
    
    uavcan_protocol_NodeStatus node_status{};
    spied_droneCAN_service.publish_node_status(node_status);
}

TEST(DroneCAN_service_publish_node_status, publish_regularly_every_configured_seconds)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    expect_one_publish_of_node_status();

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

    expect_one_publish_of_node_status();

    int ACTUAL_TIME = MILLISECONDS_BETWEEN_NODE_STATUS_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

TEST(DroneCAN_service_publish_node_status, is_time_to_publish_again)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    expect_one_publish_of_node_status();
    int ACTUAL_TIME = MILLISECONDS_BETWEEN_NODE_STATUS_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);

    expect_one_publish_of_node_status();
    ACTUAL_TIME = 2*MILLISECONDS_BETWEEN_NODE_STATUS_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

//The system is sending the right node status data
TEST(DroneCAN_service_publish_node_status, right_data_is_sent) {
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    uavcan_protocol_NodeStatus node_status{};
    int ACTUAL_TIME_IN_MILLISECONDS = 10000;
    node_status.uptime_sec = ACTUAL_TIME_IN_MILLISECONDS/1000;
    node_status.vendor_specific_status_code = NODE_STATUS_VENDOR_SPECIFIC_STATUS_CODE;
    
    mock().expectOneCall("uavcan_protocol_NodeStatus_encode")
          .withParameterOfType("uavcan_protocol_NodeStatus", "msg", (const void*)&node_status);

    mock().expectOneCall("canard->broadcast")
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();

    droneCAN_service.run_pending_tasks(ACTUAL_TIME_IN_MILLISECONDS);
}