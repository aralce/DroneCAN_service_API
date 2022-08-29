#ifndef AUXILIARY_FUNCTIONS_H_
#define AUXILIARY_FUNCTIONS_H_
#include <cstring>
#include <uavcan.protocol.param.Value.h>
#include <uavcan.protocol.param.NumericValue.h>

#define UAVCAN_PARAM_VALUE_MAX_NAME_LENGTH 128

uavcan_protocol_param_Value package_uavcan_param_value(int32_t value_to_package);
uavcan_protocol_param_Value package_uavcan_param_value(int64_t value_to_package);
uavcan_protocol_param_Value package_uavcan_param_value(float value_to_package);
uavcan_protocol_param_Value package_uavcan_param_value(bool value_to_package);

uavcan_protocol_param_Value package_uavcan_param_value_string(const uint8_t* value_to_package);
uavcan_protocol_param_Value package_uavcan_param_value_string(const char* value_to_package);

uavcan_protocol_param_Value package_uavcan_param_value_empty();

uavcan_protocol_param_NumericValue package_uavcan_param_numeric_value(int32_t value_to_package);
uavcan_protocol_param_NumericValue package_uavcan_param_numeric_value(float value_to_package);

uavcan_protocol_param_NumericValue package_uavcan_param_numeric_value_empty();

//DEBUG
#include <Arduino.h> 
#include <rom/rtc.h> //DEBUG

#define CPU0_RESETS_CPU1 RESET_REASON_CPU1_CPU0
inline void println_only_after_hardware_reset(const char* msg) {
    if (esp_rom_get_reset_reason(0) == RESET_REASON_CHIP_POWER_ON && esp_rom_get_reset_reason(1) == CPU0_RESETS_CPU1)
        Serial2.println(msg);
}

#endif // AUXILIARY_FUNCTIONS_H_