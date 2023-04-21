#pragma once
/**
 * CAN DRIVER CONFIGURATION
 * 
 * Micro-controller and Framework:
 * ------------------------------
 * Leave uncommented the option that fits your need.
 * In the case of more than one option uncommented, the used option will be the upper one
 * 
 * **/

#define USE_ESP32_WITH_ARDUINO
#define USE_ESP32_WITH_ESP_IDF










































































////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
/**DO NOT CHANGE THIS *******/
#if defined(USE_ESP32_WITH_ARDUINO)
    #define ESP32_WITH_ARDUINO
#elif defined(USE_ESP32_WITH_ESP_IDF)
    #define ESP32_WITH_ESP_IDF
#endif
/**DO NOT CHANGE THIS *******/
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////