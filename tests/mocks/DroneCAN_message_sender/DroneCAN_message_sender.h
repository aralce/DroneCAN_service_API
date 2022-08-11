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
    #include "Canard_wrapper.h"
#endif
#include "DroneCAN_service_configuration.h"

enum class DroneCAN_error{
    ON_INITIALIZATION,
    FAIL_ON_PUBLISH,
};

typedef void (*droneCAN_handle_error_t)(DroneCAN_error error);

class DroneCAN_message_sender {
public:
    explicit DroneCAN_message_sender(Canard& canard, CAN_bus_adaptor& can_driver, droneCAN_handle_error_t handle_error = nullptr);
    //     mock().actualCall("DroneCAN_message_sender->constructor")
    //           .withParameter("canard", (const void*)&canard)
    //           .withParameter("can_driver", (const void*)&can_driver)
    //           .withPointerParameter("handle_error", (void*)handle_error);
    // }

    // bool is_healthy() {
    //     mock().actualCall("DroneCAN_message_sender->is_healthy");
    //     return mock().returnBoolValueOrDefault(true);
    // }

    // template <typename UAVCAN_MESSAGE>
    // void broadcast_message(UAVCAN_MESSAGE& uavcan_message) {
    //     mock().actualCall("DroneCAN_message_sender->broadcast_message")
    //           .withPointerParameter("uavcan_message", (void*)&uavcan_message);
    // }

    // void send_response_message(uavcan_protocol_param_GetSetResponse& param_response, uint8_t destination_node_id) {
    //     mock().actualCall("DroneCAN_message_sender->send_response_message")
    //           .withParameter("param_response", (const void*)&param_response);
    // }
};

#endif // DRONECAN_SERVICE_BASE_H_