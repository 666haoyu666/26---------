/**
 * @file  osal_mutex.c
 * @brief FreeRTOS mutex wrapper; mutexes are forbidden in ISR context.
 */

#include "osal_mutex.h"

#include "osal_error.h"
#include "osal_freertos_priv.h"

int32_t osal_mutex_create(osal_mutex_handle_t *mutex_handle)
{
    SemaphoreHandle_t handle;

    if (mutex_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }

    *mutex_handle = NULL;
    handle = xSemaphoreCreateMutex();
    if (handle == NULL) {
        return OSAL_ERROR;
    }

    *mutex_handle = (osal_mutex_handle_t)handle;
    return OSAL_SUCCESS;
}

void osal_mutex_delete(osal_mutex_handle_t mutex_handle)
{
    if (mutex_handle == NULL) {
        return;
    }

    vSemaphoreDelete((SemaphoreHandle_t)mutex_handle);
}

int32_t osal_mutex_give(osal_mutex_handle_t mutex_handle)
{
    BaseType_t status;

    if (mutex_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }
    if (OSAL_IS_IN_ISR()) {
        return OSAL_ERR_IN_ISR;
    }

    status = xSemaphoreGive((SemaphoreHandle_t)mutex_handle);
    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_ERROR;
}

int32_t osal_mutex_take(osal_mutex_handle_t mutex_handle,
                        osal_tick_type_t timeout_ms)
{
    BaseType_t status;

    if (mutex_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }
    if (OSAL_IS_IN_ISR()) {
        return OSAL_ERR_IN_ISR;
    }

    status = xSemaphoreTake((SemaphoreHandle_t)mutex_handle,
                            osal_ms_to_ticks(timeout_ms));
    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_ERROR_TIMEOUT;
}
