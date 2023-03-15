#include "free_rtos.h"

#define pdMS_TO_TICKS(x) ms_to_ticks(x)
#define portTICK_PERIOD_MS 1

uint64_t ms_to_ticks(uint64_t milliseconds) {
    mock().actualCall("pdMS_TO_TICKS")
          .withUnsignedLongLongIntParameter("milliseconds", milliseconds);
    return mock().returnLongLongIntValueOrDefault(0);
}

void vTaskDelay(const uint64_t ticks_to_delay) {
    mock().actualCall("vTaskDelay")
          .withUnsignedLongLongIntParameter("ticks_to_delay", ticks_to_delay);
}

uint64_t xTaskGetTickCount() {
    mock().actualCall("xTaskGetTickCount");
    return mock().returnLongLongIntValueOrDefault(0);
}

esp_freertos_tick_cb_t tick_callback_function = nullptr;

uint8_t esp_register_freertos_tick_hook(esp_freertos_tick_cb_t cb_function) {
    mock().actualCall("esp_register_freertos_tick_hook")
          .withFunctionPointerParameter("cb_function", cb_function);
    
    tick_callback_function = cb_function;
    return mock().returnUnsignedIntValueOrDefault(0);
}

esp_freertos_tick_cb_t spy_tick_callback_function() {
    return tick_callback_function;
}