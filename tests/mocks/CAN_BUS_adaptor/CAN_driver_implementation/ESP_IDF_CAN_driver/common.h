#pragma once
#include <stdint.h>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#define IRAM_ATTR

inline void esp_restart() {
    mock().actualCall("esp_restart");
}

#define ESP_LOGI(TAG, format, message) esp_log_info(TAG, format, message)
inline void esp_log_info(const char* tag, const char*, const char* message) {
    mock().actualCall("ESP_LOGI")
          .withStringParameter("tag", tag)
          .withStringParameter("message", message);
}

#define ESP_LOGE(TAG, format, message) esp_log_error(TAG, format, message)
inline void esp_log_error(const char* tag, const char*, const char* message) {
    mock().actualCall("ESP_LOGE")
          .withStringParameter("tag", (const char*)tag)
          .withStringParameter("message", (const char*)message);
}


#define ESP_LOG_INFO 1
#define ESP_LOG_IS_NOT_INFO 2

inline void esp_log_level_set(const char* tag, uint8_t level) {
    mock().actualCall("esp_log_level_set")
          .withStringParameter("tag", tag)
          .withUnsignedIntParameter("level", (uint8_t)level);
}

inline uint8_t esp_log_level_get(const char* tag) {
    mock().actualCall("esp_log_level_get")
          .withStringParameter("tag", tag);
    return (uint8_t)mock().returnUnsignedIntValueOrDefault(0);
}