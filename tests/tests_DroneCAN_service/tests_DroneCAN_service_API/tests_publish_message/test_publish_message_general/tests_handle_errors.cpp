#include "../common_to_publish_message/common_to_publish_message.h"

void handle_error_fail_publishing(DroneCAN_error error)
{
    mock().actualCall("handle_error_fail_publishing")
          .withIntParameter("error", (int)error);
}

TEST_GROUP(DroneCAN_service_publish_with_error)
{
    Canard_message_type_info_t_comparator type_info_comparator;
    Canard_message_data_t_comparator data_comparator;
    CanardCANFrame_comparator can_frame_comparator;
    void setup()
    {
        mock().installComparator("canard_message_type_info_t", type_info_comparator);
        mock().installComparator("canard_message_data_t", data_comparator);
        mock().installComparator("CanardCANFrame", can_frame_comparator);
    }
    void teardown()
    {
        execute_test_teardown();
    }
};


TEST(DroneCAN_service_publish_with_error, error_on_canard_broadcast)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls(handle_error_fail_publishing);

    const int16_t BROADCAST_FAILURE = -1;
    mock().expectOneCall("canard->broadcast")
          .ignoreOtherParameters()
          .andReturnValue(BROADCAST_FAILURE);

    mock().expectOneCall("handle_error_fail_publishing")
          .withIntParameter("error", (int)DroneCAN_error::FAIL_ON_PUBLISH);

    mock().ignoreOtherCalls();
    publish_batteryInfo_message(droneCAN_service);
}



TEST(DroneCAN_service_publish_with_error, error_on_send_CAN_message)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls(handle_error_fail_publishing);

    const int FAILURE_ON_SEND =  0;
    
    mock().expectOneCall("canard->is_txQueue_empty")
          .andReturnValue(false);

    mock().expectOneCall("CAN_bus_adaptor->send_frame")
          .ignoreOtherParameters()
          .andReturnValue(FAILURE_ON_SEND);
    
    mock().expectOneCall("handle_error_fail_publishing")
          .withIntParameter("error", (int)DroneCAN_error::FAIL_ON_PUBLISH);

    mock().expectOneCall("canard->is_txQueue_empty")
          .andReturnValue(true);        
    
    mock().ignoreOtherCalls();

    publish_batteryInfo_message(droneCAN_service);
}