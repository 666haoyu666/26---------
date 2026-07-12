#ifndef OSAL_TIMER_H
#define OSAL_TIMER_H

#include "common_types.h"

typedef void (*osal_timer_cb_function_t)(osal_timer_handle_t timer_handle,
                                         void *argument);

int32_t osal_timer_create(osal_timer_handle_t *timer_handle,
                          const char *timer_name,
                          osal_tick_type_t timer_period_ms,
                          uint8_t auto_reload,
                          osal_timer_cb_function_t timer_cb,
                          void *argument);

int32_t osal_timer_start(osal_timer_handle_t timer_handle,
                         osal_tick_type_t timeout_ms);

int32_t osal_timer_stop(osal_timer_handle_t timer_handle,
                        osal_tick_type_t timeout_ms);

int32_t osal_timer_period_change(osal_timer_handle_t timer_handle,
                                 osal_tick_type_t new_period_ms,
                                 osal_tick_type_t timeout_ms);

int32_t osal_timer_delete(osal_timer_handle_t timer_handle,
                          osal_tick_type_t timeout_ms);

int32_t osal_timer_reset(osal_timer_handle_t timer_handle,
                         osal_tick_type_t timeout_ms);

osal_tick_type_t osal_timer_period_get(osal_timer_handle_t timer_handle);

#endif /* OSAL_TIMER_H */
