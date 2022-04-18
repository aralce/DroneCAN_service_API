#include <Arduino.h>
#include <DroneCAN_service.h>

/*
    This example shows how to publish a battery message once with a handle error callback function.
    The batteryInfo message must be provided with the correct data.

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

uavcan_equipment_power_BatteryInfo& get_battery_info_message() {}

void setup() {
    Serial.begin(9600);
    DroneCAN_service droneCAN_service(handle_error);
}

void loop() {
    uavcan_equipment_power_BatteryInfo battery_info_message = get_battery_info_message();

    delay(TIME_BETWEEN_BATTERY_INFO_PUBLISHES);
}