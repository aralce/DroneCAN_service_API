#include <Arduino.h>
#include <DroneCAN_service.h>

/*
    This example shows how to publish a battery message once with no handle error callback function.
    The batteryInfo message must be provided with the correct data.

    THIS EXAMPLE IS NOT FUNCTIONAL. ONLY SHOWS HOW TO USE THE LIBRARY
*/

#define TIME_BETWEEN_BATTERY_INFO_PUBLISHES

uavcan_equipment_power_BatteryInfo& get_battery_info_message() {}

void setup() {
    Serial.begin(9600);

    DroneCAN_service droneCAN_service;
    if (!droneCAN_service.is_healthy())
        Serial.println("Error initializing droneCAN_service");
}

void loop() {
    uavcan_equipment_power_BatteryInfo battery_info_message = get_battery_info_message();
    
    droneCAN_service.publish_message(battery_info_message);
    if (!droneCAN_service.is_healthy())
        Serial.println("Error publishing batteryInfo");

    delay(TIME_BETWEEN_BATTERY_INFO_PUBLISHES);
}