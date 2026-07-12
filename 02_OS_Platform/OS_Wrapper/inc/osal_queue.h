#ifndef OSAL_QUEUE_H
#define OSAL_QUEUE_H

#include "common_types.h"

int32_t osal_queue_create(size_t queue_depth, size_t data_size,
                          osal_queue_handle_t *queue_handle);

void osal_queue_delete(osal_queue_handle_t queue_handle);

int32_t osal_queue_send(osal_queue_handle_t queue_handle, const void *data,
                        osal_tick_type_t timeout_ms);

int32_t osal_queue_receive(osal_queue_handle_t queue_handle, void *data,
                           osal_tick_type_t timeout_ms);

int32_t osal_queue_peek(osal_queue_handle_t queue_handle, void *data,
                        osal_tick_type_t timeout_ms);

int32_t osal_queue_msg_waiting(osal_queue_handle_t queue_handle);

#endif /* OSAL_QUEUE_H */
