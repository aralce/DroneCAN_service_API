#ifndef DRONECAN_SERVICE_BASE_H_
#define DRONECAN_SERVICE_BASE_H_

#include <cstdio>
#include <cstdint>
#ifdef IS_RUNNING_TESTS
    #include <Canard_wrapper.h>
    #include <CAN_bus_adaptor.h>
    #include <uavcan.equipment.power.BatteryInfo.h>
    #include <uavcan.protocol.NodeStatus.h>
#else
    #include "Canard_wrapper.h"
    #include "CAN_bus_adaptor.h"
    #include "uavcan.equipment.power.BatteryInfo.h"
    #include "uavcan.protocol.NodeStatus.h"
#endif
#include "DroneCAN_service_configuration.h"

enum class DroneCAN_error{
    ON_INITIALIZATION,
    FAIL_ON_PUBLISH,
};

typedef void (*droneCAN_handle_error_t)(DroneCAN_error error);

class DroneCAN_message_sender {
public:
    explicit DroneCAN_message_sender(uint8_t node_ID, droneCAN_handle_error_t handle_error = dummy_function);

protected:
    Canard canard{LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, UAVCAN_MAX_BYTES_ON_MESSAGE};
    CAN_bus_adaptor can_driver;

    droneCAN_handle_error_t _handle_error;
    bool _is_healthy = true;
    static void dummy_function(DroneCAN_error error) {}

    uint8_t _node_ID;
    
    void publish_generic_message(canard_message_type_info_t type_info, canard_message_data_t data);
    
    void try_initialize_CAN_bus_driver();
    void try_broadcast_with_canard(canard_message_type_info_t type_info, canard_message_data_t data);
    void try_send_CAN_bus_frame(CanardCANFrame& frame);
    void send_pending_CAN_frames();
};

#endif