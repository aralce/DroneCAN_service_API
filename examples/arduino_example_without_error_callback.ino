#include <Arduino.h>
#include <DroneCAN_service.h>

/*
    //**DISCLAIMER THIS EXAMPLE COULD BE OUTDATED//
    
    This example shows how to publish a battery message with no handle error callback function.
    The batteryInfo message must be provided with the correct data.

    The node node ID assigned is DEFAULT_NODE id, which It can be found on configuration.h file

    THIS EXAMPLE IS NOT FUNCTIONAL. ONLY SHOWS HOW TO USE THE LIBRARY
*/

#define TIME_BETWEEN_BATTERY_INFO_PUBLISHES

uavcan_equipment_power_BatteryInfo& get_battery_info_message() {/**battery_info must be a static variable**/}

void setup() {
    Serial.begin(9600);

    DroneCAN_service droneCAN_service;
    if (!droneCAN_service.is_healthy())
        Serial.println("Error initializing droneCAN_service");
    
    uavcan_equipment_power_BatteryInfo battery_info_message = get_battery_info_message();
    const int MILLIS_BETWEEN_PUBLISHES_OF_BATTERY_INFO = 10000;
    droneCAN_service.publish_regularly(battery_info_message, MILLIS_BETWEEN_PUBLISHES_OF_BATTERY_INFO);
}

void loop() {
    uint32_t actual_time = millis();
    dronCAN_service.run_pending_tasks(actual_time);
    
    if (!droneCAN_service.is_healthy())
        Serial.println("Error publishing batteryInfo");

    if (do_i_want_to_stop_publish_battery_info())
        publish_regularly(battery_info_message, 0);
}
