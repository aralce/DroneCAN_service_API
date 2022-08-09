#include "common_to_publish_message.h"

void publish_batteryInfo_message(DroneCAN_service &droneCAN_service)
{
    uavcan_equipment_power_BatteryInfo batteryInfo;
    droneCAN_service.publish_message(batteryInfo); 
}