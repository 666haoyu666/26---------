/**
 * @file  osal_sema.c
 * @brief FreeRTOS semaphore wrapper with task and ISR adaptation.
 */

#include "osal_sema.h"

#include "osal_freertos_priv.h"

platform_err_t osal_sema_counting_create(osal_sema_handle_t *sema_handle,
                                         uint32_t max_count,
                                         uint32_t init_count)
{
    SemaphoreHandle_t handle;

    if (sema_handle == NULL) {
        return PLATFORM_ERR_PARAM;
    }
    if (max_count == 0U || init_count > max_count) {
        return PLATFORM_ERR_PARAM;
    }

    *sema_handle = NULL;
    handle = xSemaphoreCreateCounting((UBaseType_t)max_count,
                                      (UBaseType_t)init_count);
    if (handle == NULL) {
        return PLATFORM_ERR_NO_MEMORY;
    }

    *sema_handle = (osal_sema_handle_t)handle;
    return PLATFORM_ERR_OK;
}

platform_err_t osal_sema_binary_create(osal_sema_handle_t *sema_handle)
{
    SemaphoreHandle_t handle;

    if (sema_handle == NULL) {
        return PLATFORM_ERR_PARAM;
    }

    *sema_handle = NULL;
    handle = xSemaphoreCreateBinary();
    if (handle == NULL) {
        return PLATFORM_ERR_NO_MEMORY;
    }

    *sema_handle = (osal_sema_handle_t)handle;
    return PLATFORM_ERR_OK;
}

void osal_sema_delete(osal_sema_handle_t sema_handle)
{
    if (sema_handle == NULL) {
        return;
    }

    vSemaphoreDelete((SemaphoreHandle_t)sema_handle);
}

platform_err_t osal_sema_give(osal_sema_handle_t sema_handle)
{
    SemaphoreHandle_t handle = (SemaphoreHandle_t)sema_handle;
    BaseType_t task_woken = pdFALSE;
    BaseType_t status;

    if (sema_handle == NULL) {
        return PLATFORM_ERR_PARAM;
    }

    if (OSAL_IS_IN_ISR()) {
        status = xSemaphoreGiveFromISR(handle, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        status = xSemaphoreGive(handle);
    }

    return (status == pdPASS) ? PLATFORM_ERR_OK : PLATFORM_ERR_FAIL;
}

platform_err_t osal_sema_take(osal_sema_handle_t sema_handle,
                              osal_tick_type_t timeout_ms)
{
    SemaphoreHandle_t handle = (SemaphoreHandle_t)sema_handle;
    BaseType_t task_woken = pdFALSE;
    BaseType_t status;

    if (sema_handle == NULL) {
        return PLATFORM_ERR_PARAM;
    }

    if (OSAL_IS_IN_ISR()) {
        status = xSemaphoreTakeFromISR(handle, &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        status = xSemaphoreTake(handle,
                                osal_ms_to_ticks(timeout_ms));
    }

    return (status == pdPASS) ? PLATFORM_ERR_OK : PLATFORM_ERR_TIMEOUT;
}
