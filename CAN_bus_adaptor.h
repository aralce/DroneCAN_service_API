#pragma once
#include <canard.h>

/**This file has a prototype for the interface of the CAN bus driver the DroneCAN service API
 * uses.
 * 
 * There are 2 options:
 * 1. Create a child class that implements the functions
 * 2. Implement the functions in this class adding a .cpp file.
 **/

class CAN_bus_adaptor {
public:
    CAN_bus_adaptor() {}
    virtual ~CAN_bus_adaptor() {}

    /**
     * @brief Send frame with CAN bus
     * 
     * This function uses a CanardCANFrame to send a frame through the CAN bus.
     * @param can_frame frame to send through CAN bus
     * @return True if the frame was sent successfully. False otherwise.
    */
    virtual bool send_frame(CanardCANFrame& can_frame) = 0;

    /**
     * @brief Read frame with CAN bus
     * 
     * This function read the frame from the CAN bus FIFO. It is expected there is some kind of
     * buffer available to store received frames.
     * 
     * @return An empty frame if there is no frame available. Return the read frame otherwise.
    */
    virtual CanardCANFrame read_frame() = 0;
};