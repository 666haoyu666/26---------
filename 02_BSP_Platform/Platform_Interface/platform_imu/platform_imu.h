/**
 * @file    platform_imu.h
 * @brief   惯性测量单元能力接口
 * @note    仅提供yaw、pitch和roll姿态角，角度单位为deg
 * @author  haoyu
 */

#ifndef PLATFORM_IMU_H
#define PLATFORM_IMU_H

#define IMU_YAW_MIN   -180.0f  /* yaw角下界，deg */
#define IMU_YAW_MAX    180.0f  /* yaw角上界，deg */
#define IMU_PITCH_MIN  -90.0f  /* pitch角下界，deg */
#define IMU_PITCH_MAX   90.0f  /* pitch角上界，deg */
#define IMU_ROLL_MIN  -180.0f  /* roll角下界，deg */
#define IMU_ROLL_MAX   180.0f  /* roll角上界，deg */

#include "platform_error.h"

/** IMU运行状态。 */
typedef enum {
    PLATFORM_IMU_OFF = 0,
    PLATFORM_IMU_ACTIVE,
    PLATFORM_IMU_ERROR
} platform_imu_run_state_t;

/** IMU姿态一致快照。 */
typedef struct {
    float                    yaw_deg;    /* 航向角，顺时针为正，deg */
    float                    pitch_deg;  /* 俯仰角，deg */
    float                    roll_deg;   /* 横滚角，deg */
    platform_imu_run_state_t run_state;  /* 模块运行状态 */
} platform_imu_data_t;

/**
 * @brief  初始化并启动IMU采样
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_ALREADY_INIT / PLATFORM_ERR_FAIL
 */
platform_err_t platform_imu_init(void);

/**
 * @brief  停止IMU采样并释放资源
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_FAIL
 */
platform_err_t platform_imu_deinit(void);

/**
 * @brief  读取同一采样时刻的IMU姿态快照
 * @param  data 输出数据，不可为NULL
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM /
 *         PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_FAIL
 * @note   读取失败时不修改输出内容
 */
platform_err_t platform_imu_read(platform_imu_data_t *data);

#endif /* PLATFORM_IMU_H */
