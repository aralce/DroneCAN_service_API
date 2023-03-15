#pragma once
#include <stdint.h>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#define pdMS_TO_TICKS(x) ms_to_ticks(x)
#define portTICK_PERIOD_MS 1

uint64_t ms_to_ticks(uint64_t milliseconds);

void vTaskDelay(const uint64_t ticks_to_delay);

uint64_t xTaskGetTickCount();

typedef void (*esp_freertos_tick_cb_t)(void);
extern esp_freertos_tick_cb_t tick_callback_function;

uint8_t esp_register_freertos_tick_hook(esp_freertos_tick_cb_t cb_function);

esp_freertos_tick_cb_t spy_tick_callback_function();