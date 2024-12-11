#pragma once
#include <DroneCAN_service_API.h>

class DroneCAN_service_spy 
{
public:
    DroneCAN_service_spy(DroneCAN_service *service_to_spy)
    : droneCAN_service(service_to_spy) {}

    CAN_driver_interface* spy_bus_adaptor() { return &droneCAN_service->can_driver;}
    droneCAN_handle_error_t spy_handle_error() { return droneCAN_service->_handle_error;}

    droneCAN_handle_error_t get_pointer_to_handle_error_function()
    {
        return droneCAN_service->_handle_error;
    }

    uavcan_protocol_NodeStatus* spy_node_status_struct()
    {
        return &droneCAN_service->nodeStatus_struct;
    }

    uavcan_protocol_GetNodeInfoResponse& get_nodeInfo()
    {
        return droneCAN_service->nodeInfo;
    }

private:
    DroneCAN_service *droneCAN_service = nullptr;
};

class CAN_bus_custom : public CAN_driver_interface
{

};