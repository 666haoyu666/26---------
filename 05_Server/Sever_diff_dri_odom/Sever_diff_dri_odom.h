/**
 * @file    Sever_diff_dri_odom.h
 * @brief   差速底盘里程计服务接口
 * @note    基于左右轮原始累计计数解算平面位姿与底盘速度
 */

#ifndef SEVER_DIFF_DRI_ODOM_H
#define SEVER_DIFF_DRI_ODOM_H

#include "platform_error.h"
#include "platform_pmotor.h"

/** 里程计运行状态。 */
typedef enum {
    PLATFORM_ODOM_OFF = 0,
    PLATFORM_ODOM_ACTIVE,
    PLATFORM_ODOM_ERROR
} server_odom_mode_t;

/** 差速底盘里程计装配配置。 */
typedef struct {
    platform_pmotor_id_t  left_id;        /* 左轮物理电机槽位 */
    platform_pmotor_id_t  right_id;       /* 右轮物理电机槽位 */
    float                 left_mm_tick;   /* 左轮每tick位移，正数 */
    float                 right_mm_tick;  /* 右轮每tick位移，正数 */
    float                 track_mm;       /* 左右轮中心距，正数 */
    int8_t                left_sign;      /* 前进计数符号：-1或1 */
    int8_t                right_sign;     /* 前进计数符号：-1或1 */
} server_odom_cfg_t;

/** 平面位姿，用于里程计初值设置。 */
typedef struct {
    float  x_mm;       /* 世界坐标系X位置 */
    float  y_mm;       /* 世界坐标系Y位置 */
    float  yaw_deg;    /* 航向角，范围[-180, 180) */
} server_odom_pose_t;

/** 差速底盘里程计一致状态快照。 */
typedef struct {
    float               x_mm;       /* 世界坐标系X位置 */
    float               y_mm;       /* 世界坐标系Y位置 */
    float               yaw_deg;    /* 航向角，范围[-180, 180) */
    float               vx_mm_s;    /* 底盘前向速度 */
    float               vy_mm_s;    /* 横向速度，差速底盘恒为0 */
    float               w_deg_s;    /* 航向角速度 */
    server_odom_mode_t  mode;       /* 当前运行状态 */
} server_odom_state_t;

/**
 * @brief  装配里程计并清零状态，保持未运行
 * @param  cfg 装配配置，不可为NULL；尺寸须为有限正数，符号须为-1或1
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM /
 *         PLATFORM_ERR_ALREADY_INIT
 */
platform_err_t server_odom_init(const server_odom_cfg_t *cfg);

/**
 * @brief  读取当前左右轮累计计数作为基线并开始解算
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_NOT_INITIALIZED /
 *         PLATFORM_ERR_BUSY / PLATFORM_ERR_FAIL
 */
platform_err_t server_odom_start(void);

/**
 * @brief  解除里程计装配并清除内部状态
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_NOT_INITIALIZED /
 *         PLATFORM_ERR_BUSY
 */
platform_err_t server_odom_deinit(void);

/**
 * @brief  校准当前平面位姿并清零瞬时速度
 * @param  pose 新位姿，不可为NULL，各分量须为有限值
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM /
 *         PLATFORM_ERR_NOT_INITIALIZED
 * @note   输入航向角会归一化到[-180, 180)
 */
platform_err_t server_odom_set_pose(const server_odom_pose_t *pose);

/**
 * @brief  读取同一解算时刻的里程计状态快照
 * @param  state 输出状态，不可为NULL
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM /
 *         PLATFORM_ERR_NOT_INITIALIZED
 * @note   读取失败时不修改输出内容
 */
platform_err_t server_odom_get(server_odom_state_t *state);

#endif /* SEVER_DIFF_DRI_ODOM_H */
