#ifndef OSAL_TASK_H
#define OSAL_TASK_H

#include "common_types.h"

typedef uint32_t osal_priority_t;
typedef void *osal_stackptr_t;
typedef void (*osal_task_entry)(void *argument);

int32_t osal_task_create(const char *task_name,
                         osal_task_entry entry,
                         size_t stack_size,
                         osal_priority_t priority,
                         osal_task_handle_t *task_handle,
                         void *argument);

void osal_task_delete(osal_task_handle_t task_handle);

void osal_task_start(void);

void osal_task_suspend(osal_task_handle_t task_handle);

void osal_task_resume(osal_task_handle_t task_handle);

void osal_task_suspend_all(void);

void osal_task_delay(uint32_t ticks);

void osal_task_delay_ms(uint32_t delay_ms);

void osal_enter_critical(void);

void osal_exit_critical(void);

int32_t osal_port_yield(void);

void osal_task_enable_interrupts(void);

void osal_task_disable_interrupts(void);

osal_tick_type_t osal_task_get_tick_count(void);

osal_task_handle_t osal_task_get_current_handle(void);

#endif /* OSAL_TASK_H */
