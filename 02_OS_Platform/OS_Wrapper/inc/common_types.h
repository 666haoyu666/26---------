#ifndef OSAL_COMMON_TYPES_H
#define OSAL_COMMON_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "osal_config.h"

#if (OSAL_USE_16_BIT_TICKS == 1U)
#define OSAL_MAX_DELAY UINT16_MAX
typedef uint16_t osal_tick_type_t;
#else
#define OSAL_MAX_DELAY UINT32_MAX
typedef uint32_t osal_tick_type_t;
#endif

typedef int32_t osal_base_type_t;
typedef void *osal_task_handle_t;
typedef void *osal_sema_handle_t;
typedef void *osal_mutex_handle_t;
typedef void *osal_queue_handle_t;
typedef void *osal_timer_handle_t;

#define OSAL_TRUE  ((osal_base_type_t)1)
#define OSAL_FALSE ((osal_base_type_t)0)

#endif /* OSAL_COMMON_TYPES_H */
