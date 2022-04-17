#pragma once
#include <cstdio>
#include <Canard_wrapper.h>
#include <CAN_bus_adaptor.h>
#include "DroneCAN_service_configuration.h"
#include <uavcan.equipment.power.BatteryInfo.h>

enum class DroneCAN_error{
    THERE_IS_NO_ERROR,
    ON_INITIALIZATION,
    FAIL_ON_PUBLISH,
};

typedef void (*droneCAN_handle_error_t)(DroneCAN_error error);

class DroneCAN_service {
public:
    explicit DroneCAN_service(droneCAN_handle_error_t handle_error = dummy_function);
    bool is_healthy();
    void publish_message(uavcan_equipment_power_BatteryInfo& battery_info);

protected:
    droneCAN_handle_error_t _handle_error;
    static void dummy_function(DroneCAN_error error) {}

private:
    Canard<LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, UAVCAN_MAX_BYTES_ON_MESSAGE> canard;
    CAN_bus_adaptor can_driver;
    bool _is_healthy = false;
    
    void try_initialize_CAN_bus_driver();
    void try_send_CAN_bus_frame(CanardCANFrame& frame);
    void send_pending_CAN_frames();
};
