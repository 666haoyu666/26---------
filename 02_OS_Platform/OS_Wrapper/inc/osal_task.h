#ifndef OSAL_TASK_H
#define OSAL_TASK_H

#include "common_types.h"
#include "platform_error.h"

typedef uint32_t osal_priority_t;
typedef void *osal_stackptr_t;
typedef void (*osal_task_entry)(void *argument);

platform_err_t osal_task_create(const char *task_name,
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

/**
 * @brief  按绝对唤醒时刻维持固定任务周期
 * @param  last_wake 上次唤醒tick，首次调用前初始化为当前tick
 * @param  period_ms 周期，ms；须大于0且不为OSAL_MAX_DELAY
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM /
 *         PLATFORM_ERR_NOT_SUPPORTED
 */
platform_err_t osal_task_wait_until(osal_tick_type_t *last_wake,
                                    uint32_t period_ms);

void osal_enter_critical(void);

void osal_exit_critical(void);

platform_err_t osal_port_yield(void);

void osal_task_enable_interrupts(void);

void osal_task_disable_interrupts(void);

osal_tick_type_t osal_task_get_tick_count(void);

osal_task_handle_t osal_task_get_current_handle(void);

#endif /* OSAL_TASK_H */
