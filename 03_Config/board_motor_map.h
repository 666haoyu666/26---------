/**
 * @file    board_motor_map.h
 * @brief   板级电机资源映射声明
 * @note    定义必须严格来源于00_Docs/01_资源分配表
 */

#ifndef BOARD_MOTOR_MAP_H
#define BOARD_MOTOR_MAP_H

#include "pwm_motor_handler.h"

/** 固定物理槽位映射；未装配槽位必须显式enabled=0。 */
extern const pwm_motor_map_t
    g_pwm_motor_map[PLATFORM_PMOTOR_ID_MAX];

/** handler使用的控制定时器和映射表装配配置。 */
extern const pwm_handler_cfg_t g_pwm_hnd_cfg;

#endif /* BOARD_MOTOR_MAP_H */
