#pragma once

/* This class is a library interface.
 *
 * The class requires to implement the read and write operations
 * over CAN.
 *
 * It is expected the user of the library to implement a CAN_driver class
 * that inherits from this class.
 */


#include "../implementation/uavcan_driver/canard.h"

class CAN_driver_interface {
public:

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