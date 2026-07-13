#ifndef OSAL_SEMA_H
#define OSAL_SEMA_H

#include "common_types.h"
#include "platform_error.h"

platform_err_t osal_sema_counting_create(osal_sema_handle_t *sema_handle,
                                         uint32_t max_count,
                                         uint32_t init_count);

platform_err_t osal_sema_binary_create(osal_sema_handle_t *sema_handle);

void osal_sema_delete(osal_sema_handle_t sema_handle);

platform_err_t osal_sema_give(osal_sema_handle_t sema_handle);

platform_err_t osal_sema_take(osal_sema_handle_t sema_handle,
                              osal_tick_type_t timeout_ms);

#endif /* OSAL_SEMA_H */
