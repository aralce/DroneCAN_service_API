#include "task.h"
TaskFunction_t callback_on_task = nullptr;

static uint8_t value_to_point; //this value is to have something different to nullptr
TaskHandle_t task_handler = &value_to_point;

void* parameter_passed_to_task_function = nullptr;