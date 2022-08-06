#pragma once
#include <DroneCAN_service_API.h>

class Spied_droneCAN_service : public DroneCAN_service {
public:
    explicit Spied_droneCAN_service(uint8_t node_ID = DEFAULT_NODE_ID, droneCAN_handle_error_t handle_error = dummy_function)
        : DroneCAN_service(node_ID, handle_error) {}

    droneCAN_handle_error_t get_pointer_to_handle_error_function() {
        return _handle_error;
    }

    droneCAN_handle_error_t get_handle_error_DUMMY() {
        return dummy_function;
    }

    void publish_node_status(uavcan_protocol_NodeStatus& node_status) {
        publish_message(node_status);
    }

    void respond_with_parameter_data(uint8_t parameter_index_from_0) {
        DroneCAN_service::respond_with_parameter_data(parameter_index_from_0);
    }
};