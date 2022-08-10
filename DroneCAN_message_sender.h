#ifndef DRONECAN_SERVICE_BASE_H_
#define DRONECAN_SERVICE_BASE_H_

#include <cstdio>
#include <cstdint>
#ifdef IS_RUNNING_TESTS
    #include <Canard_wrapper.h>
    #include <CAN_bus_adaptor.h>
    #include <DSDL_to_canard_DTO.h>
#else
    #include "Canard_wrapper.h"
    #include "CAN_bus_adaptor.h"
#endif
#include "DroneCAN_service_configuration.h"

enum class DroneCAN_error{
    ON_INITIALIZATION,
    FAIL_ON_PUBLISH,
};

typedef void (*droneCAN_handle_error_t)(DroneCAN_error error);

class DroneCAN_message_sender {
public:
    explicit DroneCAN_message_sender(droneCAN_handle_error_t handle_error = dummy_function);

    bool is_healthy() const {return _is_healthy;}
    
    void broadcast_message(DSDL_to_canard_DTO& data_transfer_object);

    template <typename UAVCAN_MESSAGE>
    void broadcast_message(UAVCAN_MESSAGE& uavcan_message) {
        DSDL_to_canard_DTO data_transfer_object(uavcan_message);
        broadcast_message(data_transfer_object);
    }

    void send_response_message(uavcan_protocol_param_GetSetResponse& param_response);

private:
    Canard canard{LIBCANARD_ALLOCATION_BUFFER_IN_BYTES, UAVCAN_MAX_BYTES_ON_MESSAGE};
    CAN_bus_adaptor can_driver;

    droneCAN_handle_error_t _handle_error;
    bool _is_healthy = true;
    static void dummy_function(DroneCAN_error error) {}
    
    void try_initialize_CAN_bus_driver();
    void try_broadcast_with_canard(canard_message_type_info_t& type_info, canard_message_data_t& data);
    void try_send_response_message_with_canard(canard_message_type_info_t& type_info, canard_message_data_t& data);
    void try_send_CAN_bus_frame(CanardCANFrame& frame);
    void send_pending_CAN_frames();
};

#endif