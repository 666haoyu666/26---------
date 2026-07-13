/**
 * @file    platform_lsensor.h
 * @brief   平行二值灰度传感器组接口
 * @note    槽位号从左到右递增，线偏差以阵列中心为原点，右正左负，单位mm
 */

#ifndef PLATFORM_LSENSOR_H
#define PLATFORM_LSENSOR_H

#include "platform_error.h"
#include "platform_type.h"

/** 灰度传感器组运行状态。 */
typedef enum {
    PLATFORM_LSENSOR_OFF = 0,
    PLATFORM_LSENSOR_ACTIVE,
    PLATFORM_LSENSOR_ERROR
} platform_lsensor_run_state_t;

/**
 * @brief  灰度传感器组对当前线位置的识别结果
 * @note   只有TRACKING状态下line_offset_mm有效；NO_LINE和AMBIGUOUS
 *         状态下的偏差不得直接用于循迹闭环控制
 */
typedef enum {
    PLATFORM_LSENSOR_NO_LINE = 0,  /* 未检测到可用线 */
    PLATFORM_LSENSOR_TRACKING,     /* 唯一连续线簇，偏差有效 */
    PLATFORM_LSENSOR_AMBIGUOUS     /* 多线簇或全路触发，偏差不唯一 */
} platform_lsensor_track_t;

/** 同一采样时刻的灰度传感器组快照。 */
typedef struct {
    float                         line_offset_mm;  /* 线中心偏差，右正左负 */
    platform_lsensor_track_t      track_state;     /* 线位置识别结果 */
    platform_lsensor_run_state_t  run_state;       /* 模块运行状态 */
} platform_lsensors_state_t;

/**
 * @brief  初始化灰度传感器组
 * @note   初始化完成后保持ACTIVE状态，也可以用于处理ERROR
 *          理论上来说不会进入ERROR状态，除非硬件异常或初始化失败
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_ALREADY_INIT / PLATFORM_ERR_FAIL
 */
platform_err_t platform_lsensor_init(void);

/**
 * @brief  注销灰度传感器组并释放资源，一般也不用可以先不实现,调用会进入OFF状态
 * @note   注销后再次使用必须重新初始化
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_FAIL
 */
platform_err_t platform_lsensor_deinit(void);

/**
 * @brief  读取灰度传感器组的同拍状态和线中心偏差
 * @param  state 输出快照，不可为NULL
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM /
 *         PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_FAIL
 * @note   无线和歧义仍返回OK；读取失败时不修改输出内容
 */
platform_err_t platform_lsensor_read(platform_lsensors_state_t *state);

#endif /* PLATFORM_LSENSOR_H */
