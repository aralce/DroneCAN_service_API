#include <common_to_DroneCAN_service_tests.h>
#include <DroneCAN_message_sender.h>

TEST_GROUP(DroneCAN_service_API)
{
    void teardown()
    {
        execute_test_teardown();
    }
};

TEST(DroneCAN_service_API, publish_batteryInfo_message)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    uavcan_equipment_power_BatteryInfo battery_info{};
    mock().expectOneCall("DroneCAN_message_sender->broadcast_message")
          .withPointerParameter("uavcan_message", (void*)&battery_info);

    droneCAN_service.publish_message(battery_info);
}

TEST(DroneCAN_service_API, publish_node_status_message)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    uavcan_protocol_NodeStatus node_status{};
    mock().expectOneCall("DroneCAN_message_sender->broadcast_message")
          .withPointerParameter("uavcan_message", (void*)&node_status);

    droneCAN_service.publish_message(node_status);
}