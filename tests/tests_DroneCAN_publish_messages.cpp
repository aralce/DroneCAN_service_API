#include "common_to_all_tests.h"
#include <uavcan.equipment.power.BatteryInfo.h>

static DroneCAN_service get_droneCAN_instance_omiting_mock_calls()
{
    mock().disable();
    DroneCAN_service droneCAN_service;
    mock().enable();
    return droneCAN_service;
}

void publish_batteryInfo_message(DroneCAN_service &droneCAN_service)
{
    uavcan_equipment_power_BatteryInfo batteryInfo;
    droneCAN_service.publish_message(batteryInfo); 
}

TEST_GROUP(DroneCAN_service_publish_batteryInfo)
{
    DroneCAN_service droneCAN_service = get_droneCAN_instance_omiting_mock_calls();
    Canard_message_type_info_t_comparator type_info_comparator;
    Canard_message_data_t_comparator data_comparator;

    void setup()
    {
        mock().installComparator("canard_message_type_info_t", type_info_comparator);
        mock().installComparator("canard_message_data_t", data_comparator);
    }

    void teardown()
    {
        teardown_mocks();
        mock().removeAllComparatorsAndCopiers();
    }
};

TEST(DroneCAN_service_publish_batteryInfo, encode_function_is_called_with_battery_message)
{
    uavcan_equipment_power_BatteryInfo battery_message;
    mock().expectOneCall("uavcan_equipment_power_BatteryInfo_encode")
          .withParameter("msg", (void*)&battery_message)
          .ignoreOtherParameters();          
    mock().ignoreOtherCalls();

    droneCAN_service.publish_message(battery_message);
}

TEST(DroneCAN_service_publish_batteryInfo, canardBroadcast_is_called_with_right_type_info)
{
    canard_message_type_info_t type_info = {.signature = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_SIGNATURE,
                                            .id = UAVCAN_EQUIPMENT_POWER_BATTERYINFO_ID,
                                            .priority = CANARD_TRANSFER_PRIORITY_LOW };
    mock().expectOneCall("broadcast")
          .withParameterOfType("canard_message_type_info_t", "type_info", (const void*)&type_info)
          .ignoreOtherParameters();
    mock().ignoreOtherCalls();

    publish_batteryInfo_message(droneCAN_service); 
}

TEST(DroneCAN_service_publish_batteryInfo, canard_broadcast_is_called_with_right_data)
{
    const uint32_t ENCODED_MESSAGE_LENGTH = 3; 
    uint8_t message_buffer[ENCODED_MESSAGE_LENGTH] = {12,13,14};
    mock().expectOneCall("uavcan_equipment_power_BatteryInfo_encode")
          .withOutputParameterReturning("buffer", message_buffer, ENCODED_MESSAGE_LENGTH)
          .ignoreOtherParameters()
          .andReturnValue(ENCODED_MESSAGE_LENGTH);

    canard_message_data_t data = {.ptr = (void*)message_buffer,
                                  .length = ENCODED_MESSAGE_LENGTH };
    mock().expectOneCall("broadcast")
          .withParameterOfType("canard_message_data_t", "data", (const void*)&data)
          .ignoreOtherParameters();
    
    publish_batteryInfo_message(droneCAN_service);
}