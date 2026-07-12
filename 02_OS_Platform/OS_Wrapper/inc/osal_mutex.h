#ifndef OSAL_MUTEX_H
#define OSAL_MUTEX_H

#include "common_types.h"

int32_t osal_mutex_create(osal_mutex_handle_t *mutex_handle);

void osal_mutex_delete(osal_mutex_handle_t mutex_handle);

int32_t osal_mutex_give(osal_mutex_handle_t mutex_handle);

int32_t osal_mutex_take(osal_mutex_handle_t mutex_handle,
                        osal_tick_type_t timeout_ms);

#endif /* OSAL_MUTEX_H */
