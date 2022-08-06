#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <canard.h>

#define UAVCAN_PROTOCOL_PARAM_EMPTY_MAX_SIZE 0
#define UAVCAN_PROTOCOL_PARAM_EMPTY_SIGNATURE (0x6C4D0E8EF37361DFULL)

struct uavcan_protocol_param_Empty {
};