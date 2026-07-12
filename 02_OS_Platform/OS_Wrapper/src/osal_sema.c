/**
 * @file  osal_sema.c
 * @brief FreeRTOS semaphore wrapper with task and ISR adaptation.
 */

#include "osal_sema.h"

#include "osal_error.h"
#include "osal_freertos_priv.h"

int32_t osal_sema_counting_create(osal_sema_handle_t *sema_handle,
                                  uint32_t max_count,
                                  uint32_t init_count)
{
    SemaphoreHandle_t handle;

    if (sema_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }
    if (max_count == 0U || init_count > max_count) {
        return OSAL_INVALID_SEM_VALUE;
    }

    *sema_handle = NULL;
    handle = xSemaphoreCreateCounting((UBaseType_t)max_count,
                                      (UBaseType_t)init_count);
    if (handle == NULL) {
        return OSAL_ERROR;
    }

    *sema_handle = (osal_sema_handle_t)handle;
    return OSAL_SUCCESS;
}

int32_t osal_sema_binary_create(osal_sema_handle_t *sema_handle)
{
    SemaphoreHandle_t handle;

    if (sema_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }

    *sema_handle = NULL;
    handle = xSemaphoreCreateBinary();
    if (handle == NULL) {
        return OSAL_ERROR;
    }

    *sema_handle = (osal_sema_handle_t)handle;
    return OSAL_SUCCESS;
}

void osal_sema_delete(osal_sema_handle_t sema_handle)
{
    if (sema_handle == NULL) {
        return;
    }

    vSemaphoreDelete((SemaphoreHandle_t)sema_handle);
}

int32_t osal_sema_give(osal_sema_handle_t sema_handle)
{
    SemaphoreHandle_t handle = (SemaphoreHandle_t)sema_handle;
    BaseType_t task_woken = pdFALSE;
    BaseType_t status;

    if (sema_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }

    if (OSAL_IS_IN_ISR()) {
        status = xSemaphoreGiveFromISR(handle, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        status = xSemaphoreGive(handle);
    }

    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_SEM_FAILURE;
}

int32_t osal_sema_take(osal_sema_handle_t sema_handle,
                       osal_tick_type_t timeout_ms)
{
    SemaphoreHandle_t handle = (SemaphoreHandle_t)sema_handle;
    BaseType_t task_woken = pdFALSE;
    BaseType_t status;

    if (sema_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }

    if (OSAL_IS_IN_ISR()) {
        status = xSemaphoreTakeFromISR(handle, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        status = xSemaphoreTake(handle,
                                osal_ms_to_ticks(timeout_ms));
    }

    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_SEM_TIMEOUT;
}
