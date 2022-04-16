
#pragma once
#include <DroneCAN_service.h>

class Spied_droneCAN_service : public DroneCAN_service {
public:
    Spied_droneCAN_service() : DroneCAN_service() {}
    
    // uint8_t* get_ptr_of_canard_transmission_buffer() {
    //     return canard_transmission_buffer;
    // }
    
    // uint32_t get_size_of_canard_transmission_buffer() {
    //     return sizeof(canard_transmission_buffer);
    // }
};