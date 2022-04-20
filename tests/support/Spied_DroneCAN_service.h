#pragma once
#include <DroneCAN_service_API.h>

class Spied_droneCAN_service : public DroneCAN_service {
public:
    explicit Spied_droneCAN_service(droneCAN_handle_error_t handle_error = dummy_function) : DroneCAN_service(handle_error) {}

    droneCAN_handle_error_t get_pointer_to_handle_error_function() {
        return _handle_error;
    }

    droneCAN_handle_error_t get_handle_error_DUMMY() {
        return dummy_function;
    }
};