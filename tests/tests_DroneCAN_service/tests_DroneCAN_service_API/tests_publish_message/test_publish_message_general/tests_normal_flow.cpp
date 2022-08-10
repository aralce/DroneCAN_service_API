#include <uavcan.equipment.power.BatteryInfo.h>
#include "../common_to_publish_message/common_to_publish_message.h"

TEST_GROUP(DroneCAN_service_publish)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
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