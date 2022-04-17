#include "common_to_publish_message/common_to_publish_message.h"

TEST_GROUP(DroneCAN_service_publish_with_error)
{
    CanardCANFrame_comparator can_frame_comparator;
    void setup()
    {
        mock().installComparator("CanardCANFrame", can_frame_comparator);
    }
    void teardown()
    {
        execute_test_teardown();
    }
};


void handle_error_fail_publishing(DroneCAN_error error)
{
    mock().actualCall("handle_error_fail_publishing")
          .withIntParameter("error", (int)error);
}

TEST(DroneCAN_service_publish_with_error, error_on_send_CAN_message)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls(handle_error_fail_publishing);

    const int FAILURE_ON_SEND =  0;
    
    mock().expectOneCall("is_txQueue_empty")
          .andReturnValue((int)false);

    mock().expectOneCall("send_frame")
          .ignoreOtherParameters()
          .andReturnValue(FAILURE_ON_SEND);
    
    mock().expectOneCall("handle_error_fail_publishing")
          .withIntParameter("error", (int)DroneCAN_error::FAIL_ON_PUBLISH);

    mock().expectOneCall("is_txQueue_empty")
          .andReturnValue((int)true);        
    
    mock().ignoreOtherCalls();

    publish_batteryInfo_message(droneCAN_service);
}