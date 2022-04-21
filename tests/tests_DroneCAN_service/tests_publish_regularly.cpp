#include <common_to_all_tests.h>

const int MILLISECONDS_BETWEEN_PUBLISHES = 10000;

void expect_one_published_message()
{
    mock().expectOneCall("broadcast")
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

    uavcan_equipment_power_BatteryInfo battery_message;
    droneCAN_service.publish_regularly(battery_message, MILLISECONDS_BETWEEN_PUBLISHES);
}

TEST(DroneCAN_service_publish_regularly, is_not_time_to_publish_again)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    expect_one_published_message();

    uavcan_equipment_power_BatteryInfo battery_message;
    droneCAN_service.publish_regularly(battery_message, MILLISECONDS_BETWEEN_PUBLISHES);
    
    const int ACTUAL_TIME = MILLISECONDS_BETWEEN_PUBLISHES - 1;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

TEST(DroneCAN_service_publish_regularly, is_time_to_publish_again)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    expect_one_published_message();

    uavcan_equipment_power_BatteryInfo battery_message;
    droneCAN_service.publish_regularly(battery_message, MILLISECONDS_BETWEEN_PUBLISHES);
    
    expect_one_published_message();

    const int ACTUAL_TIME = MILLISECONDS_BETWEEN_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

TEST(DroneCAN_service_publish_regularly, if_is_time_to_publish_again_only_publish_one_time)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

    expect_one_published_message();

    uavcan_equipment_power_BatteryInfo battery_message;
    droneCAN_service.publish_regularly(battery_message, MILLISECONDS_BETWEEN_PUBLISHES);

    expect_one_published_message();

    const int ACTUAL_TIME = MILLISECONDS_BETWEEN_PUBLISHES;
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
    droneCAN_service.run_pending_tasks(ACTUAL_TIME);
}

// TEST(DroneCAN_service_publish_regularly, register_a_message_twice_use_time_of_last_registered_message)
// {
//     DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();

//     expect_one_published_message();
//     uavcan_equipment_power_BatteryInfo battery_message;
//     droneCAN_service.publish_regularly(battery_message, MILLISECONDS_BETWEEN_PUBLISHES);
    
//     expect_one_published_message();
//     droneCAN_service.publish_regularly(battery_message, MILLISECONDS_BETWEEN_PUBLISHES - 4);

//     expect_one_published_message();
//     const int ACTUAL_TIME = MILLISECONDS_BETWEEN_PUBLISHES - 4;
//     droneCAN_service.run_pending_tasks(ACTUAL_TIME);
// }