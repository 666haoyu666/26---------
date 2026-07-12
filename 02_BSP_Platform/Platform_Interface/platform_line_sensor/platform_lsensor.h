/**
 * @file    platform_lsensor.h
 * @brief   线性传感器能力接口，使用稳定物理槽位标识
 * @note    line_map的位n对应PLATFORM_LSENSOR_n，置1表示检测到线
 */

#ifndef PLATFORM_LSENSOR_H
#define PLATFORM_LSENSOR_H

/** 统一的线性传感器逻辑值。 */
#define PLATFORM_NOT_LINE 0  /* 未检测到线 */
#define PLATFORM_IS_LINE  1  /* 检测到线 */

#include "platform_error.h"

/** 稳定物理线性传感器槽位，共8个槽位。 */
typedef enum {
    PLATFORM_LSENSOR_0 = 0,
    PLATFORM_LSENSOR_1,
    PLATFORM_LSENSOR_2,
    PLATFORM_LSENSOR_3,
    PLATFORM_LSENSOR_4,
    PLATFORM_LSENSOR_5,
    PLATFORM_LSENSOR_6,
    PLATFORM_LSENSOR_7,
    PLATFORM_LSENSOR_ID_MAX
} platform_lsensor_id_t;

/**
 * @brief  将传感器槽位转换为line_map位掩码
 * @param  id 稳定物理槽位，必须小于PLATFORM_LSENSOR_ID_MAX
 */
#define PLATFORM_LSENSOR_BIT(id) \
    ((uint8_t)(1U << (uint8_t)(id)))

/** 线性传感器运行状态。 */
typedef enum {
    PLATFORM_LSENSOR_OFF = 0,
    PLATFORM_LSENSOR_ACTIVE,
    PLATFORM_LSENSOR_ERROR
} platform_lsensor_run_state_t;

/** 一致状态快照。 */
typedef struct {
    uint8_t                      line_map;   /* 当前传感器组位图 */
    platform_lsensor_run_state_t run_state;  /* 模块运行状态 */
} platform_lsensors_state_t;

/**
 * @brief  初始化全部线性传感器
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_ALREADY_INIT / PLATFORM_ERR_FAIL
 */
platform_err_t platform_lsensor_init(void);

/**
 * @brief  注销全部线性传感器并释放资源
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_FAIL
 */
platform_err_t platform_lsensor_deinit(void);

/**
 * @brief  读取线性传感器组的当前值
 * @param  lsensors_state 输出值，不可为NULL
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM /
 *         PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_NOT_SUPPORTED
 * @note   读取失败时不修改输出内容
 */
platform_err_t platform_lsensor_read(platform_lsensors_state_t *lsensors_state);

#endif /* PLATFORM_LSENSOR_H */
