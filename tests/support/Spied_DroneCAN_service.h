#pragma once
#include <DroneCAN_service_API.h>

class Spied_droneCAN_service : public DroneCAN_service {
public:
    explicit Spied_droneCAN_service(CAN_bus_adaptor& can_driver, uint8_t node_ID = DEFAULT_NODE_ID, droneCAN_handle_error_t handle_error = nullptr)
      : DroneCAN_service(can_driver, node_ID, handle_error)
    {

    }

    droneCAN_handle_error_t get_pointer_to_handle_error_function()
    {
        return _handle_error;
    }

    uavcan_protocol_NodeStatus* spy_node_status_struct()
    {
        return &nodeStatus_struct;
    }
};