/**
 * @file  osal_timer.c
 * @brief FreeRTOS software timer wrapper and callback adaptation.
 */

#include <string.h>

#include "osal_timer.h"

#include "osal_config.h"
#include "osal_error.h"
#include "osal_freertos_priv.h"

typedef struct {
    char                      timer_name[OSAL_NAME_MAX_LEN];
    osal_timer_cb_function_t  func;
    void                     *arg;
    osal_timer_handle_t       timer_handle;
} osal_timer_record_t;

static void osal_timer_cb(TimerHandle_t native_timer)
{
    osal_timer_record_t *record;

    record = (osal_timer_record_t *)pvTimerGetTimerID(native_timer);
    if (record == NULL || record->func == NULL) {
        return;
    }

    record->func(record->timer_handle, record->arg);
}

static void osal_timer_record_free(void *record, uint32_t unused)
{
    (void)unused;
    vPortFree(record);
}

int32_t osal_timer_create(osal_timer_handle_t *timer_handle,
                          const char *timer_name,
                          osal_tick_type_t timer_period_ms,
                          uint8_t auto_reload,
                          osal_timer_cb_function_t timer_cb,
                          void *argument)
{
    osal_timer_record_t *record;
    TimerHandle_t native_timer;
    size_t name_len;

    if (timer_handle == NULL || timer_name == NULL || timer_cb == NULL) {
        return OSAL_INVALID_POINTER;
    }
    if (memchr(timer_name, '\0', OSAL_NAME_MAX_LEN) == NULL) {
        return OSAL_ERR_NAME_TOO_LONG;
    }
    if (timer_period_ms == 0U) {
        return OSAL_ERR_INVALID_SIZE;
    }
    if (auto_reload > 1U) {
        return OSAL_ERR_INVALID_ARGUMENT;
    }

    *timer_handle = NULL;
    record = pvPortMalloc(sizeof(*record));
    if (record == NULL) {
        return OSAL_TIMER_ERR_UNAVAILABLE;
    }

    name_len = strlen(timer_name);
    memset(record, 0, sizeof(*record));
    memcpy(record->timer_name, timer_name, name_len + 1U);
    record->func = timer_cb;
    record->arg = argument;

    native_timer = xTimerCreate(record->timer_name,
                                osal_ms_to_ticks(timer_period_ms),
                                (UBaseType_t)auto_reload,
                                record,
                                osal_timer_cb);
    if (native_timer == NULL) {
        vPortFree(record);
        return OSAL_TIMER_ERR_UNAVAILABLE;
    }

    *timer_handle = (osal_timer_handle_t)native_timer;
    record->timer_handle = *timer_handle;
    return OSAL_SUCCESS;
}

int32_t osal_timer_start(osal_timer_handle_t timer_handle,
                         osal_tick_type_t timeout_ms)
{
    BaseType_t task_woken = pdFALSE;
    BaseType_t status;

    if (timer_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }

    if (OSAL_IS_IN_ISR()) {
        status = xTimerStartFromISR((TimerHandle_t)timer_handle,
                                    &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        status = xTimerStart((TimerHandle_t)timer_handle,
                             osal_ms_to_ticks(timeout_ms));
    }

    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_ERROR;
}

int32_t osal_timer_stop(osal_timer_handle_t timer_handle,
                        osal_tick_type_t timeout_ms)
{
    BaseType_t task_woken = pdFALSE;
    BaseType_t status;

    if (timer_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }

    if (OSAL_IS_IN_ISR()) {
        status = xTimerStopFromISR((TimerHandle_t)timer_handle,
                                   &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        status = xTimerStop((TimerHandle_t)timer_handle,
                            osal_ms_to_ticks(timeout_ms));
    }

    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_ERROR;
}

int32_t osal_timer_period_change(osal_timer_handle_t timer_handle,
                                 osal_tick_type_t new_period_ms,
                                 osal_tick_type_t timeout_ms)
{
    TickType_t new_period;
    BaseType_t task_woken = pdFALSE;
    BaseType_t status;

    if (timer_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }
    if (new_period_ms == 0U) {
        return OSAL_ERR_INVALID_SIZE;
    }

    new_period = osal_ms_to_ticks(new_period_ms);
    if (OSAL_IS_IN_ISR()) {
        status = xTimerChangePeriodFromISR((TimerHandle_t)timer_handle,
                                           new_period,
                                           &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        status = xTimerChangePeriod((TimerHandle_t)timer_handle,
                                    new_period,
                                    osal_ms_to_ticks(timeout_ms));
    }

    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_ERROR;
}

int32_t osal_timer_delete(osal_timer_handle_t timer_handle,
                          osal_tick_type_t timeout_ms)
{
    TimerHandle_t native_timer = (TimerHandle_t)timer_handle;
    void *record;
    TickType_t wait_ticks;
    BaseType_t status;

    if (timer_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }
    if (OSAL_IS_IN_ISR()) {
        return OSAL_ERR_IN_ISR;
    }

    wait_ticks = osal_ms_to_ticks(timeout_ms);
    record = pvTimerGetTimerID(native_timer);
    status = xTimerDelete(native_timer, wait_ticks);
    if (status != pdPASS) {
        return OSAL_ERROR;
    }

    status = xTimerPendFunctionCall(osal_timer_record_free,
                                    record,
                                    0U,
                                    wait_ticks);
    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_ERROR;
}

int32_t osal_timer_reset(osal_timer_handle_t timer_handle,
                         osal_tick_type_t timeout_ms)
{
    BaseType_t task_woken = pdFALSE;
    BaseType_t status;

    if (timer_handle == NULL) {
        return OSAL_INVALID_POINTER;
    }

    if (OSAL_IS_IN_ISR()) {
        status = xTimerResetFromISR((TimerHandle_t)timer_handle,
                                    &task_woken);
        portYIELD_FROM_ISR(task_woken);
    } else {
        status = xTimerReset((TimerHandle_t)timer_handle,
                             osal_ms_to_ticks(timeout_ms));
    }

    return (status == pdPASS) ? OSAL_SUCCESS : OSAL_ERROR;
}

osal_tick_type_t osal_timer_period_get(osal_timer_handle_t timer_handle)
{
    TickType_t period_ticks;

    if (timer_handle == NULL) {
        return 0U;
    }

    period_ticks = xTimerGetPeriod((TimerHandle_t)timer_handle);
    return osal_ticks_to_ms(period_ticks);
}
