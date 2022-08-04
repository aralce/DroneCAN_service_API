#include <common_to_all_tests.h>

const int MILLISECONDS_BETWEEN_PUBLISHES = 10000;

uavcan_equipment_power_BatteryInfo& get_battery_info()
{
    mock().actualCall("get_battery_info");
    static uavcan_equipment_power_BatteryInfo battery_info{};
    return battery_info;
}

void expect_one_published_message()
{
    mock().expectOneCall("get_battery_info");
    mock().expectOneCall("uavcan_equipment_power_BatteryInfo_encode")
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();
}

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

TEST(DroneCAN_service_publish_regularly, register_a_message_to_publish_trigger_publish_of_the_message)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    expect_one_published_message();

    droneCAN_service.publish_regularly(get_battery_info, MILLISECONDS_BETWEEN_PUBLISHES);
}

TEST(DroneCAN_service_publish_regularly, is_not_time_to_publish_again)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    expect_one_published_message();

    droneCAN_service.publish_regularly(get_battery_info, MILLISECONDS_BETWEEN_PUBLISHES);
    
    const int ACTUAL_TIME = MILLISECONDS_BETWEEN_PUBLISHES - 1;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

TEST(DroneCAN_service_publish_regularly, is_time_to_publish_again)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    expect_one_published_message();

    droneCAN_service.publish_regularly(get_battery_info, MILLISECONDS_BETWEEN_PUBLISHES);
    
    expect_one_published_message();

    const int ACTUAL_TIME = MILLISECONDS_BETWEEN_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

TEST(DroneCAN_service_publish_regularly, if_is_time_to_publish_again_only_publish_one_time)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    expect_one_published_message();

    droneCAN_service.publish_regularly(get_battery_info, MILLISECONDS_BETWEEN_PUBLISHES);

    expect_one_published_message();

    const int ACTUAL_TIME = MILLISECONDS_BETWEEN_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

TEST(DroneCAN_service_publish_regularly, register_a_message_twice_use_time_of_last_registered_message)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    expect_one_published_message();
    droneCAN_service.publish_regularly(get_battery_info, MILLISECONDS_BETWEEN_PUBLISHES);
    
    expect_one_published_message();
    droneCAN_service.publish_regularly(get_battery_info, MILLISECONDS_BETWEEN_PUBLISHES - 4);

    expect_one_published_message();
    const int ACTUAL_TIME = MILLISECONDS_BETWEEN_PUBLISHES - 4;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

TEST(DroneCAN_service_publish_regularly, check_pending_task_when_there_is_no_register_message)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    mock().expectNoCall("get_battery_info");
    mock().expectNoCall("uavcan_equipment_power_BatteryInfo_encode");
    mock().ignoreOtherCalls();

    const int ACTUAL_TIME = MILLISECONDS_BETWEEN_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

//-if_null_time_is_registered_to_regular_publish_the_message_is_unregistered
TEST(DroneCAN_service_publish_regularly, if_null_time_register_then_message_is_unregistered)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    expect_one_published_message();
    droneCAN_service.publish_regularly(get_battery_info, MILLISECONDS_BETWEEN_PUBLISHES);

    expect_one_published_message();
    int ACTUAL_TIME = MILLISECONDS_BETWEEN_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);

    const int MILLISECONDS_TO_UNREGISTER_MESSAGE = 0;
    droneCAN_service.publish_regularly(get_battery_info, MILLISECONDS_TO_UNREGISTER_MESSAGE);

    ACTUAL_TIME = 2*MILLISECONDS_BETWEEN_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}