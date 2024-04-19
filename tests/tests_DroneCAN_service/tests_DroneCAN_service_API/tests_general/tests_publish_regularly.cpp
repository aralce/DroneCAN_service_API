#include <common_to_DroneCAN_service_tests.h>
#include <Spied_DroneCAN_service.h>

const int MICROSECONDS_BETWEEN_PUBLISHES = 10e6;
static CAN_bus_adaptor can_driver;

TEST_GROUP(DroneCAN_service_publish_regularly)
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

TEST(DroneCAN_service_publish_regularly,
publish_regularly_the_node_status_message_without_register_when_is_time_and_only_once)
{
    mock().disable();
    Spied_droneCAN_service spied_droneCAN_service(can_driver);
    mock().enable();

    uavcan_protocol_NodeStatus* node_status = spied_droneCAN_service.spy_node_status_struct();
    mock().expectOneCall("DroneCAN_message_sender->broadcast_message")
          .withPointerParameter("uavcan_message", (void*)node_status);
    mock().ignoreOtherCalls();

    int ACTUAL_TIME = MICROSECONDS_BETWEEN_NODE_STATUS_PUBLISHES;
    spied_droneCAN_service.run_pending_tasks(ACTUAL_TIME);
    spied_droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

TEST(DroneCAN_service_publish_regularly,
WHEN_only_node_status_msg_is_sent_THEN_run_pending_tasks_return_ms_for_next_msg)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    uint32_t ms_between_msgs = MICROSECONDS_BETWEEN_NODE_STATUS_PUBLISHES/1000;
    constexpr microseconds usecs_between_msgs = MICROSECONDS_BETWEEN_NODE_STATUS_PUBLISHES;
    CHECK_EQUAL(ms_between_msgs, droneCAN_service.run_pending_tasks(0));
    CHECK_EQUAL(ms_between_msgs/2, droneCAN_service.run_pending_tasks(usecs_between_msgs/2));
    
    //publish msg
    CHECK_EQUAL(ms_between_msgs, droneCAN_service.run_pending_tasks(usecs_between_msgs));
    
    CHECK_EQUAL(ms_between_msgs/2, droneCAN_service.run_pending_tasks(3*usecs_between_msgs/2));
}

TEST(DroneCAN_service_publish_regularly, is_not_time_yet_to_publish_node_status_message)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().expectNoCall("DroneCAN_message_sender->broadcast_message");
    mock().ignoreOtherCalls();
    const microseconds INITIAL_TIME = 0; 
    droneCAN_service.run_pending_tasks(INITIAL_TIME);
}

TEST(DroneCAN_service_publish_regularly, node_status_is_sent_with_right_data)
{
    mock().disable();
    Spied_droneCAN_service spied_droneCAN_service(can_driver);
    mock().enable();
    mock().ignoreOtherCalls();

    uavcan_protocol_NodeStatus* nodeStatus = spied_droneCAN_service.spy_node_status_struct();
    mock().expectOneCall("DroneCAN_message_sender->broadcast_message")
          .ignoreOtherParameters();
    const microseconds ACTUAL_TIME = MICROSECONDS_BETWEEN_PUBLISHES;
    spied_droneCAN_service.run_pending_tasks(ACTUAL_TIME);
    
    const uint8_t NODESTATUS_HEALTH_OK = 0;
    CHECK_EQUAL(NODESTATUS_HEALTH_OK, nodeStatus->health);
    const uint8_t MODE_OPERATIONAL = 0;
    CHECK_EQUAL(MODE_OPERATIONAL, nodeStatus->mode);
    CHECK_EQUAL(ACTUAL_TIME/1e6, nodeStatus->uptime_sec);
}

TEST(DroneCAN_service_publish_regularly,
register_batteryInfo_for_regular_publish_then_after_registered_time_publish_message)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    uavcan_equipment_power_BatteryInfo battery_info{};
    const microseconds MICROSECONDS_BETWEEN_BATTERY_INFO_PUBLISH = 10e6;
    droneCAN_service.register_for_regular_publish(&battery_info,
                                                  MICROSECONDS_BETWEEN_BATTERY_INFO_PUBLISH);

    mock().expectOneCall("DroneCAN_message_sender->broadcast_message") //send node_status
          .ignoreOtherParameters();

    //doesn't publish_batteryInfo_message
    droneCAN_service.run_pending_tasks(MICROSECONDS_BETWEEN_BATTERY_INFO_PUBLISH - 1);

    const microseconds ACTUAL_TIME = MICROSECONDS_BETWEEN_BATTERY_INFO_PUBLISH;

    mock().expectOneCall("DroneCAN_message_sender->broadcast_message")
          .withPointerParameter("uavcan_message", (void*)&battery_info);

    droneCAN_service.run_pending_tasks(ACTUAL_TIME); //publish_batteryInfo_message
}

TEST(DroneCAN_service_publish_regularly,
WHEN_register_battery_Info_with_double_freq_than_nodeStatus_msg_THEN_get_time_until_next_publish)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    mock().ignoreOtherCalls();

    uavcan_equipment_power_BatteryInfo battery_info{};
    constexpr microseconds node_status_publishes = MICROSECONDS_BETWEEN_NODE_STATUS_PUBLISHES;
    constexpr microseconds MICROSECONDS_PUBLISH_BATTERY_INFO = node_status_publishes/2;
    droneCAN_service.register_for_regular_publish(&battery_info,
                                                  MICROSECONDS_PUBLISH_BATTERY_INFO);

    constexpr uint32_t ms_for_publish_batteryInfo = MICROSECONDS_PUBLISH_BATTERY_INFO/1000;

    //battery info is the half time node status publish time, so max time is batteryInfo time.
    CHECK_EQUAL(ms_for_publish_batteryInfo, droneCAN_service.run_pending_tasks(0));

    CHECK_EQUAL(ms_for_publish_batteryInfo,
                droneCAN_service.run_pending_tasks(MICROSECONDS_PUBLISH_BATTERY_INFO));
    CHECK_EQUAL(ms_for_publish_batteryInfo,
                droneCAN_service.run_pending_tasks(node_status_publishes));
}