#include <common_to_all_tests.h>
#include "../common_to_publish_message/common_to_publish_message.h"

TEST_GROUP(DroneCAN_service_publish_node_status)
{
    Canard_message_type_info_t_comparator type_info_comparator;
    void setup()
    {
        mock().installComparator("canard_message_type_info_t", type_info_comparator);
    }
    void teardown()
    {
        execute_test_teardown();
    }
};

// -publish_node_status_send_node_status_message
TEST(DroneCAN_service_publish_node_status, send_node_status_message)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    canard_message_type_info_t message_info = {.signature = UAVCAN_PROTOCOL_NODESTATUS_SIGNATURE,
                                               .id = UAVCAN_PROTOCOL_NODESTATUS_ID,
                                               .priority = CANARD_TRANSFER_PRIORITY_HIGHEST};
    mock().expectOneCall("broadcast")
          .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&message_info)
          .ignoreOtherParameters();
    
    uavcan_protocol_NodeStatus node_status{};
    droneCAN_service.publish_message(node_status);
}
// -error_when_publish_node_status_generate_FAIL_ON_PUBLISH
// -error_when_publish_makes_node_unhealthy