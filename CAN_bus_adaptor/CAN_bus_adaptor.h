#if defined ESP32
    #include "ESP32_implementation/CAN_bus_for_ESP32.h"
#elif defined TARGET_RP2040
    #include "RP2040_implementation/CAN_bus_adaptor_for_RP2040.h"
#endif
