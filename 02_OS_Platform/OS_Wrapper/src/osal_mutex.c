/**
 * @file  osal_mutex.c
 * @brief FreeRTOS mutex wrapper; mutexes are forbidden in ISR context.
 */

#include "osal_mutex.h"

#include "osal_freertos_priv.h"

platform_err_t osal_mutex_create(osal_mutex_handle_t *mutex_handle)
{
    SemaphoreHandle_t handle;

    if (mutex_handle == NULL) {
        return PLATFORM_ERR_PARAM;
    }

    *mutex_handle = NULL;
    handle = xSemaphoreCreateMutex();
    if (handle == NULL) {
        return PLATFORM_ERR_NO_MEMORY;
    }

    *mutex_handle = (osal_mutex_handle_t)handle;
    return PLATFORM_ERR_OK;
}

void osal_mutex_delete(osal_mutex_handle_t mutex_handle)
{
    if (mutex_handle == NULL) {
        return;
    }

    vSemaphoreDelete((SemaphoreHandle_t)mutex_handle);
}

platform_err_t osal_mutex_give(osal_mutex_handle_t mutex_handle)
{
    BaseType_t status;

    if (mutex_handle == NULL) {
        return PLATFORM_ERR_PARAM;
    }
    if (OSAL_IS_IN_ISR()) {
        return PLATFORM_ERR_NOT_SUPPORTED;
    }

    status = xSemaphoreGive((SemaphoreHandle_t)mutex_handle);
    return (status == pdPASS) ? PLATFORM_ERR_OK : PLATFORM_ERR_FAIL;
}

platform_err_t osal_mutex_take(osal_mutex_handle_t mutex_handle,
                               osal_tick_type_t timeout_ms)
{
    BaseType_t status;

    if (mutex_handle == NULL) {
        return PLATFORM_ERR_PARAM;
    }
    if (OSAL_IS_IN_ISR()) {
        return PLATFORM_ERR_NOT_SUPPORTED;
    }

    status = xSemaphoreTake((SemaphoreHandle_t)mutex_handle,
                            osal_ms_to_ticks(timeout_ms));
    return (status == pdPASS) ? PLATFORM_ERR_OK : PLATFORM_ERR_TIMEOUT;
}
