/**
 * @file    pwm_motor_handler.h
 * @brief   多电机handler的装配和板级映射合同
 * @note    控制回调属于handler私有实现，不向Service暴露
 */

#ifndef PWM_MOTOR_HANDLER_H
#define PWM_MOTOR_HANDLER_H

#include "platform_encoder.h"
#include "platform_io.h"
#include "platform_pmotor.h"
#include "platform_pwm.h"
#include "platform_timer.h"
#include "pwm_motor_driver.h"

/** PWM驱动板输出拓扑。 */
typedef enum {
    PWM_MOTOR_DUAL_PWM = 0,
    PWM_MOTOR_PWM_DIR
} pwm_motor_out_t;

/** 一个稳定物理电机槽位的板级映射。 */
typedef struct {
    platform_pmotor_id_t   id;               /* 稳定物理槽位 */
    uint8_t                enabled;          /* 1=已装配，0=禁用 */
    pwm_motor_out_t        out_type;         /* 输出拓扑 */
    platform_pwm_ch_t      pwm_ch_a;         /* 正转/PWM通道 */
    platform_pwm_ch_t      pwm_ch_b;         /* 反转通道或UNUSED */
    platform_io_id_t       dir_io;           /* DIR逻辑IO或UNUSED */
    uint8_t                output_invert;    /* 输出方向独立反相 */
    uint8_t                encoder_invert;   /* 编码器方向独立反相 */
    platform_encoder_id_t  enc_id;           /* 编码器逻辑标识 */
    const pwm_drv_cfg_t    *cfg;             /* 电机型号/实例配置 */
} pwm_motor_map_t;

/** handler模块级装配配置。 */
typedef struct {
    const pwm_motor_map_t  *map;        /* 固定容量映射表 */
    uint32_t               map_count;   /* 必须等于PLATFORM_PMOTOR_ID_MAX */
    platform_timer_id_t    timer_id;    /* 专用控制节拍 */
    uint32_t               period_ms;   /* 控制周期 */
} pwm_handler_cfg_t;

#endif /* PWM_MOTOR_HANDLER_H */
