#ifndef OSAL_FREERTOS_PRIV_H
#define OSAL_FREERTOS_PRIV_H

#include <stdint.h>

#include "FreeRTOS.h"
#include "cmsis_compiler.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "common_types.h"

#if (configSUPPORT_DYNAMIC_ALLOCATION != 1)
#error "OSAL currently requires FreeRTOS dynamic allocation"
#endif

#if (configUSE_MUTEXES != 1)
#error "OSAL mutex support requires configUSE_MUTEXES"
#endif

#if (configUSE_COUNTING_SEMAPHORES != 1)
#error "OSAL counting semaphores require configUSE_COUNTING_SEMAPHORES"
#endif

#if (configUSE_TIMERS != 1) || (INCLUDE_xTimerPendFunctionCall != 1)
#error "OSAL timers require timers and xTimerPendFunctionCall"
#endif

#if (INCLUDE_vTaskDelete != 1) || (INCLUDE_vTaskSuspend != 1) || \
    (INCLUDE_vTaskDelay != 1)
#error "OSAL tasks require delete, suspend and delay support"
#endif

#if (INCLUDE_vTaskDelayUntil != 1)
#error "OSAL periodic task delay requires vTaskDelayUntil support"
#endif

#if (OSAL_USE_16_BIT_TICKS != configUSE_16_BIT_TICKS)
#error "OSAL and FreeRTOS tick widths must match"
#endif

#define OSAL_IS_IN_ISR() (__get_IPSR() != 0U)

static inline TickType_t osal_ms_to_ticks(osal_tick_type_t time_ms)
{
    uint64_t ticks;

    if (time_ms == OSAL_MAX_DELAY) {
        return portMAX_DELAY;
    }

    ticks = ((uint64_t)time_ms * (uint64_t)configTICK_RATE_HZ + 999U) /
            1000U;
    if (ticks >= (uint64_t)portMAX_DELAY) {
        ticks = (uint64_t)portMAX_DELAY - 1U;
    }

    return (TickType_t)ticks;
}

static inline osal_tick_type_t osal_ticks_to_ms(TickType_t ticks)
{
    uint64_t time_ms;

    time_ms = ((uint64_t)ticks * 1000U) /
              (uint64_t)configTICK_RATE_HZ;
    if (time_ms > (uint64_t)OSAL_MAX_DELAY) {
        return OSAL_MAX_DELAY;
    }

    return (osal_tick_type_t)time_ms;
}

#endif /* OSAL_FREERTOS_PRIV_H */
