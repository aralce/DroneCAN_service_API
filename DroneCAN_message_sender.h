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
    #include "DSDL_to_canard_DTO.h"
#endif
#include "DroneCAN_service_configuration.h"

//TODO: move this somewhere
enum class DroneCAN_error{
    ON_INITIALIZATION,
    FAIL_ON_PUBLISH,
    FAIL_ON_RECEPTION
};

typedef void (*droneCAN_handle_error_t)(DroneCAN_error error);

class DroneCAN_message_sender {
public:
    explicit DroneCAN_message_sender(Canard& canard, CAN_bus_adaptor& can_driver, droneCAN_handle_error_t handle_error = nullptr);

    bool is_healthy() const {return _is_healthy;}
    
    template <typename UAVCAN_MESSAGE>
    void broadcast_message(UAVCAN_MESSAGE& uavcan_message) {
        DSDL_to_canard_DTO data_transfer_object(uavcan_message);
        broadcast_message(data_transfer_object);
    }

    void send_response_message(uavcan_protocol_param_GetSetResponse& param_response, uint8_t destination_node_id);

private:
    Canard& _canard;
    CAN_bus_adaptor& _can_driver;

    droneCAN_handle_error_t _handle_error;
    bool _is_healthy = true;
    
    void try_initialize_CAN_bus_driver();
    void try_broadcast_with_canard(canard_message_type_info_t& type_info, canard_message_data_t& data);
    void try_send_response_message_with_canard(uint8_t destination_node_id, canard_message_type_info_t& type_info, canard_message_data_t& data);
    void try_send_CAN_bus_frame(CanardCANFrame& frame);
    void send_pending_CAN_frames();

    void broadcast_message(DSDL_to_canard_DTO& data_transfer_object);
};

#endif