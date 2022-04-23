#include <Arduino.h>
#include <DroneCAN_service.h>

/*
    This example shows how to publish a battery message once with a handle error callback function.
    The batteryInfo message must be provided with the correct data.

    The node ID assigned is DEFAULT_NODE id, which It can be found on configuration.h file
    
    Every time the DroneCAN_service instance fails, the handle error function is executed.
    The error variable has the error code.

    THIS EXAMPLE IS NOT FUNCTIONAL. ONLY SHOWS HOW TO USE THE LIBRARY
*/

#define TIME_BETWEEN_BATTERY_INFO_PUBLISHES

void handle_error(DroneCAN_error error) {
    Serial.println("Error on droneCAN");
    switch (error)
    {
    case DroneCAN_error::ON_INITIALIZATION:
        Serial.println("FAIL to initialize");
        break;
    case DroneCAN_error::FAIL_ON_PUBLISH:
        Serial.println("FAIL to publish message");
        break;
    }
}

uavcan_equipment_power_BatteryInfo& get_battery_info_message() { /**your code for return battery_info**/
                                                                 /**battery_info must be a static variable**/}


void setup() {
    Serial.begin(9600);
    DroneCAN_service droneCAN_service(handle_error);
    
    uavcan_equipment_power_BatteryInfo battery_info_message = get_battery_info_message();
    const int MILLIS_BETWEEN_PUBLISHES_OF_BATTERY_INFO = 10000;
    droneCAN_service.publish_regularly(battery_info_message, MILLIS_BETWEEN_PUBLISHES_OF_BATTERY_INFO);
}

void loop() {
    uint32_t actual_time = millis();
    dronCAN_service.run_pending_tasks(actual_time);

    if (do_i_want_to_stop_publish_battery_info())
        publish_regularly(battery_info_message, 0);
}