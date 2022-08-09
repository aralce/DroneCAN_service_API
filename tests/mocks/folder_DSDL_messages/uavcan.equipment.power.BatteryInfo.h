#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <canard.h>

#define UAVCAN_EQUIPMENT_POWER_BATTERYINFO_MAX_SIZE 55
#define UAVCAN_EQUIPMENT_POWER_BATTERYINFO_SIGNATURE (0x249C26548A711966ULL)
#define UAVCAN_EQUIPMENT_POWER_BATTERYINFO_ID 1092

struct uavcan_equipment_power_BatteryInfo {
    float temperature;
    float voltage;
    float current;
    float average_power_10sec;
    float remaining_capacity_wh;
    float full_charge_capacity_wh;
    float hours_to_full_charge;
    uint16_t status_flags;
    uint8_t state_of_health_pct;
    uint8_t state_of_charge_pct;
    uint8_t state_of_charge_pct_stdev;
    uint8_t battery_id;
    uint32_t model_instance_id;
    struct { uint8_t len; uint8_t data[31]; }model_name;
};

uint32_t uavcan_equipment_power_BatteryInfo_encode(struct uavcan_equipment_power_BatteryInfo* msg, uint8_t* buffer);
