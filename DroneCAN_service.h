#pragma once
#include <cstdio>
#include <canard.h>
#include <CAN_bus_interface.h>
#include "DroneCAN_service_configuration.h"

class DroneCAN_service {
public:
    explicit DroneCAN_service();
    bool is_healthy();

private:
    CanardInstance canardInstance;
	uint8_t canardBuffer[LIBCANARD_ALLOCATION_BUFFER_IN_BYTES];
    CAN_bus_interface can_interface;
    bool _is_healthy = false;
};
