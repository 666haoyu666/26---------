/**
 * @file  osal_task.c
 * @brief FreeRTOS task and scheduler wrapper.
 */

#include <string.h>

#include "osal_task.h"

#include "osal_config.h"
#include "osal_error.h"
#include "osal_freertos_priv.h"

int32_t osal_task_create(const char *task_name,
                         osal_task_entry entry,
                         size_t stack_size,
                         osal_priority_t priority,
                         osal_task_handle_t *task_handle,
                         void *argument)
{
    configSTACK_DEPTH_TYPE stack_depth;
    BaseType_t status;

    if (task_name == NULL || entry == NULL) {
        return OSAL_INVALID_POINTER;
    }
    if (memchr(task_name, '\0', OSAL_NAME_MAX_LEN) == NULL ||
        memchr(task_name, '\0', configMAX_TASK_NAME_LEN) == NULL) {
        return OSAL_ERR_NAME_TOO_LONG;
    }
    if (stack_size == 0U) {
        return OSAL_ERR_INVALID_SIZE;
    }
    if (priority >= configMAX_PRIORITIES) {
        return OSAL_ERR_INVALID_PRIORITY;
    }

    stack_depth = (configSTACK_DEPTH_TYPE)
                  ((stack_size + sizeof(StackType_t) - 1U) /
                   sizeof(StackType_t));
    if (task_handle != NULL) {
        *task_handle = NULL;
    }

    status = xTaskCreate(entry,
                         task_name,
                         stack_depth,
                         argument,
                         (UBaseType_t)priority,
                         (TaskHandle_t *)task_handle);
    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_ERROR;
}

void osal_task_delete(osal_task_handle_t task_handle)
{
    vTaskDelete((TaskHandle_t)task_handle);
}

void osal_task_start(void)
{
    vTaskStartScheduler();
}

void osal_task_suspend(osal_task_handle_t task_handle)
{
    vTaskSuspend((TaskHandle_t)task_handle);
}

void osal_task_resume(osal_task_handle_t task_handle)
{
    if (task_handle == NULL) {
        return;
    }

    if (OSAL_IS_IN_ISR()) {
        BaseType_t task_woken;

        task_woken = xTaskResumeFromISR((TaskHandle_t)task_handle);
        portYIELD_FROM_ISR(task_woken);
    } else {
        vTaskResume((TaskHandle_t)task_handle);
    }
}

void osal_task_suspend_all(void)
{
    vTaskSuspendAll();
}

void osal_task_delay(uint32_t ticks)
{
    vTaskDelay((TickType_t)ticks);
}

void osal_task_delay_ms(uint32_t delay_ms)
{
    vTaskDelay(osal_ms_to_ticks((osal_tick_type_t)delay_ms));
}

void osal_enter_critical(void)
{
    if (OSAL_IS_IN_ISR()) {
#ifdef configASSERT
        configASSERT(0);
#else
        return;
#endif
    }

    taskENTER_CRITICAL();
}

void osal_exit_critical(void)
{
    if (OSAL_IS_IN_ISR()) {
#ifdef configASSERT
        configASSERT(0);
#else
        return;
#endif
    }

    taskEXIT_CRITICAL();
}

int32_t osal_port_yield(void)
{
    if (OSAL_IS_IN_ISR()) {
        return OSAL_ERR_IN_ISR;
    }

    portYIELD();
    return OSAL_SUCCESS;
}

void osal_task_enable_interrupts(void)
{
    taskENABLE_INTERRUPTS();
}

void osal_task_disable_interrupts(void)
{
    taskDISABLE_INTERRUPTS();
}

osal_tick_type_t osal_task_get_tick_count(void)
{
    TickType_t ticks;

    if (OSAL_IS_IN_ISR()) {
        ticks = xTaskGetTickCountFromISR();
    } else {
        ticks = xTaskGetTickCount();
    }

    return (osal_tick_type_t)ticks;
}

osal_task_handle_t osal_task_get_current_handle(void)
{
    return (osal_task_handle_t)xTaskGetCurrentTaskHandle();
}
