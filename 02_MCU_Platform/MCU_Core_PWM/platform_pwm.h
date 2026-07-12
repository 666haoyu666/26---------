/**
 * @file    platform_pwm.h
 * @brief   MCU逻辑PWM通道接口
 * @note    通道到TIM/CH的映射只存在于Impl和board map
 */

#ifndef PLATFORM_PWM_H
#define PLATFORM_PWM_H

#include "platform_error.h"

#define PLATFORM_PWM_DUTY_MAX 1000U       /* 千分比占空比上限 */
#define PLATFORM_PWM_CH_UNUSED 0xFFFFFFFFU /* 未使用逻辑通道 */

typedef uint32_t platform_pwm_ch_t;

/**
 * @brief  启动逻辑PWM通道，启动时不得产生非零危险脉冲
 * @param  ch 逻辑PWM通道
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM / PLATFORM_ERR_FAIL
 */
platform_err_t platform_pwm_start(platform_pwm_ch_t ch);

/**
 * @brief  停止逻辑PWM通道并驱动到配置的无效电平
 * @param  ch 逻辑PWM通道
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM / PLATFORM_ERR_FAIL
 */
platform_err_t platform_pwm_stop(platform_pwm_ch_t ch);

/**
 * @brief  设置千分比占空比
 * @param  ch   逻辑PWM通道
 * @param  duty 占空比，范围0~PLATFORM_PWM_DUTY_MAX
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM / PLATFORM_ERR_FAIL
 * @note    duty为0时，返回前必须保证零态已对物理输出生效
 */
platform_err_t platform_pwm_set_duty(
    platform_pwm_ch_t ch,
    uint32_t duty);

#endif /* PLATFORM_PWM_H */
