#pragma once

#include <stdint.h>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

typedef void* TaskHandle_t;
#define tskIDLE_PRIORITY			( ( UBaseType_t ) 0U )

#define taskYIELD()					portYIELD()

#define portCHAR		int8_t
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		int32_t
#define portSHORT		int16_t
#define portSTACK_TYPE	uint8_t
#define portBASE_TYPE	int

typedef portSTACK_TYPE			StackType_t;
typedef portBASE_TYPE			BaseType_t;
typedef unsigned portBASE_TYPE	UBaseType_t;

typedef void (*TaskFunction_t)( void * );

#define pdFALSE			( ( BaseType_t ) 0 )
#define pdTRUE			( ( BaseType_t ) 1 )

#define pdPASS			( pdTRUE )
#define pdFAIL			( pdFALSE )

#define tskNO_AFFINITY	( 0x7FFFFFFF )

extern TaskFunction_t callback_on_task;
extern TaskHandle_t task_handler;
extern void* parameter_passed_to_task_function;

inline BaseType_t xTaskCreatePinnedToCore(	TaskFunction_t pvTaskCode,
									const char * const pcName,
									const uint32_t usStackDepth,
									void * const pvParameters,
									UBaseType_t uxPriority,
									TaskHandle_t * const pvCreatedTask,
									const BaseType_t xCoreID) {
    mock().actualCall("xTaskCreatePinnedToCore")
          .withIntParameter("uxPriority", uxPriority)
          .withPointerParameter("pvCreatedTask", pvCreatedTask)
          .withIntParameter("xCoreID", xCoreID);
    
    callback_on_task = pvTaskCode;
    *pvCreatedTask = task_handler;
    parameter_passed_to_task_function = pvParameters;
    return mock().returnIntValueOrDefault(pdPASS);
}

inline TaskFunction_t spy_task_function() {
    return callback_on_task;
}

inline void* const spy_parameter_passed_to_task_function() {
    return parameter_passed_to_task_function;
}

inline TaskHandle_t spy_task_handler() {
    return task_handler;
}

inline void vTaskDelete( TaskHandle_t xTaskToDelete ) {
    mock().actualCall("vTaskDelete")
          .withPointerParameter("xTaskToDelete", (void*)xTaskToDelete);
}

inline UBaseType_t uxTaskPriorityGet( const TaskHandle_t xTask ) {
    mock().actualCall("uxTaskPriorityGet")
          .withPointerParameter("xTask", (void*)xTask);
    return mock().intReturnValue();
}

inline void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority ) {
    mock().actualCall("vTaskPrioritySet")
          .withPointerParameter("xTask", (void*)xTask)
          .withIntParameter("uxNewPriority", uxNewPriority);
}