/**
 * @file    platform_timer.h
 * @brief   MCU逻辑周期定时器回调接口
 * @note    回调在ISR上下文执行，必须有界、无阻塞、无日志
 */

#ifndef PLATFORM_TIMER_H
#define PLATFORM_TIMER_H

#include "platform_error.h"

#define PLATFORM_TIMER_ID_UNUSED 0xFFFFFFFFU /* 未使用逻辑定时器 */

typedef uint32_t platform_timer_id_t;
typedef void (*platform_timer_cb_t)(void *ctx);

/**
 * @brief  为停止状态的逻辑定时器注册唯一回调
 * @param  id  逻辑定时器标识
 * @param  cb  ISR回调，不可为NULL
 * @param  ctx 所有者上下文，可为NULL
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM / PLATFORM_ERR_BUSY
 * @note    仅允许在任务或主循环上下文调用
 */
platform_err_t platform_timer_register(
    platform_timer_id_t id,
    platform_timer_cb_t cb,
    void *ctx);

/**
 * @brief  注销停止状态逻辑定时器的回调和上下文
 * @param  id 逻辑定时器标识
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM / PLATFORM_ERR_BUSY
 * @note    仅允许在任务或主循环上下文调用
 */
platform_err_t platform_timer_unregister(platform_timer_id_t id);

/**
 * @brief  按毫秒周期启动逻辑定时器
 * @param  id        逻辑定时器标识
 * @param  period_ms 周期，必须大于0
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM / PLATFORM_ERR_FAIL
 * @note    仅允许在任务或主循环上下文调用
 */
platform_err_t platform_timer_start(
    platform_timer_id_t id,
    uint32_t period_ms);

/**
 * @brief  停止逻辑定时器并等待其不再分发新回调
 * @param  id 逻辑定时器标识
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM / PLATFORM_ERR_FAIL
 * @note    仅允许在任务或主循环上下文调用
 */
platform_err_t platform_timer_stop(platform_timer_id_t id);

#endif /* PLATFORM_TIMER_H */
