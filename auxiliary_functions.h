#ifndef AUXILIARY_FUNCTIONS_H_
#define AUXILIARY_FUNCTIONS_H_
#include <cstring>
#include <uavcan.protocol.param.Value.h>

uavcan_protocol_param_Value package_uavcan_param_value(int32_t value_to_package);
uavcan_protocol_param_Value package_uavcan_param_value(float value_to_package);
uavcan_protocol_param_Value package_uavcan_param_value(bool value_to_package);

uavcan_protocol_param_Value package_uavcan_param_value_string(const uint8_t* value_to_package);
uavcan_protocol_param_Value package_uavcan_param_value_string(const char* value_to_package);

uavcan_protocol_param_Value package_uavcan_param_value_empty();

#endif // AUXILIARY_FUNCTIONS_H_