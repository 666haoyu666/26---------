/**
 * @file  osal_queue.c
 * @brief FreeRTOS queue wrapper with task and ISR context adaptation.
 */

#include "osal_queue.h"

#include "osal_error.h"
#include "osal_freertos_priv.h"

int32_t osal_queue_create(size_t queue_depth, size_t data_size,
                          osal_queue_handle_t *queue_handle)
{
    QueueHandle_t handle;

    if (queue_depth == 0U || data_size == 0U) {
        return OSAL_ERR_INVALID_SIZE;
    }
    if (queue_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }

    *queue_handle = NULL;
    handle = xQueueCreate((UBaseType_t)queue_depth,
                          (UBaseType_t)data_size);
    if (handle == NULL) {
        return OSAL_ERROR;
    }

    *queue_handle = (osal_queue_handle_t)handle;
    return OSAL_SUCCESS;
}

void osal_queue_delete(osal_queue_handle_t queue_handle)
{
    if (queue_handle == NULL) {
        return;
    }

    vQueueDelete((QueueHandle_t)queue_handle);
}

int32_t osal_queue_send(osal_queue_handle_t queue_handle, const void *data,
                        osal_tick_type_t timeout_ms)
{
    QueueHandle_t handle = (QueueHandle_t)queue_handle;
    BaseType_t task_woken = pdFALSE;
    BaseType_t status;

    if (queue_handle == NULL || data == NULL) {
        return OSAL_INVALID_POINTER;
    }

    if (OSAL_IS_IN_ISR()) {
        status = xQueueSendFromISR(handle, data, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        status = xQueueSend(handle, data,
                            osal_ms_to_ticks(timeout_ms));
    }

    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_QUEUE_FULL;
}

int32_t osal_queue_receive(osal_queue_handle_t queue_handle, void *data,
                           osal_tick_type_t timeout_ms)
{
    QueueHandle_t handle = (QueueHandle_t)queue_handle;
    BaseType_t task_woken = pdFALSE;
    BaseType_t status;

    if (queue_handle == NULL || data == NULL) {
        return OSAL_INVALID_POINTER;
    }

    if (OSAL_IS_IN_ISR()) {
        status = xQueueReceiveFromISR(handle, data, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        status = xQueueReceive(handle, data,
                               osal_ms_to_ticks(timeout_ms));
    }

    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_QUEUE_EMPTY;
}

int32_t osal_queue_peek(osal_queue_handle_t queue_handle, void *data,
                        osal_tick_type_t timeout_ms)
{
    QueueHandle_t handle = (QueueHandle_t)queue_handle;
    BaseType_t status;

    if (queue_handle == NULL || data == NULL) {
        return OSAL_INVALID_POINTER;
    }

    if (OSAL_IS_IN_ISR()) {
        status = xQueuePeekFromISR(handle, data);
    } else {
        status = xQueuePeek(handle, data,
                            osal_ms_to_ticks(timeout_ms));
    }

    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_QUEUE_EMPTY;
}

int32_t osal_queue_msg_waiting(osal_queue_handle_t queue_handle)
{
    QueueHandle_t handle = (QueueHandle_t)queue_handle;

    if (queue_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }
    if (OSAL_IS_IN_ISR()) {
        return (int32_t)uxQueueMessagesWaitingFromISR(handle);
    }

    return (int32_t)uxQueueMessagesWaiting(handle);
}
