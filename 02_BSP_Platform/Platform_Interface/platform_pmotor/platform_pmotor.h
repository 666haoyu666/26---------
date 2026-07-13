/**
 * @file    platform_pmotor.h
 * @brief   闭环电机能力接口，使用稳定物理槽位标识
 * @note    Service只设置目标和读取状态，控制节拍由handler持有
 * @author  haoyu
 */

#ifndef PLATFORM_PMOTOR_H
#define PLATFORM_PMOTOR_H

#include "platform_error.h"
#include "platform_type.h"

/** 稳定物理电机槽位，不承载左右轮等底盘语义。 */
typedef enum {
    PLATFORM_PMOTOR_0 = 0,
    PLATFORM_PMOTOR_1,
    PLATFORM_PMOTOR_2,
    PLATFORM_PMOTOR_3,
    PLATFORM_PMOTOR_ID_MAX
} platform_pmotor_id_t;

/** 电机模块运行状态。 */
typedef enum {
    PLATFORM_PMOTOR_OFF = 0,
    PLATFORM_PMOTOR_ACTIVE,
    PLATFORM_PMOTOR_ERROR
} platform_pmotor_run_state_t;

/** 基础故障位；后续可追加，不改变已有位值。 */
typedef enum {
    PLATFORM_PMOTOR_FLT_NONE = 0U
    //PLATFORM_PMOTOR_FLT_CFG  = (1U << 0)
} platform_pmotor_fault_t;

/** 单电机一致状态快照。 */
typedef struct {
    float                       target_rps;     /* 当前目标转速 */
    float                       rps;            /* 滤波后输出轴转速 */
    int64_t                     position;       /* 原始累计tick */
    platform_pmotor_run_state_t run_state;      /* 模块运行状态 */
    platform_pmotor_fault_t     fault_flags;    /* 锁存故障位 */
    int32_t                     applied_duty;   /* 已应用占空比 */
} platform_pmotor_state_t;

/** 电机目标状态为多电机伪原子操作设计 */
typedef struct {
    platform_pmotor_id_t id;   /* 稳定物理槽位 */
    float                rps;  /* 输出轴目标转速 */
} platform_pmotor_target_t;

/**
 * @brief  装配全部电机并注册内部控制回调，还是OFF态
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_ALREADY_INIT / PLATFORM_ERR_FAIL
 */
platform_err_t platform_pmotor_init(void);

/**
 * @brief  注销控制回调并释放全部电机资源
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_FAIL
 */
platform_err_t platform_pmotor_deinit(void);

/**
 * @brief  使能全部电机，保留资源和累计位置，同时进入ACTIVE状态
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_FAIL
 */
platform_err_t platform_pmotor_start(void);

/**
 * @brief  失能全部电机并滑行，保留资源和累计位置，同时进入OFF状态
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_FAIL
 */
platform_err_t platform_pmotor_stop(void);

/**
 * @brief  设置一个物理电机的目标转速，自动使能对应电机
 * @param  id  稳定物理槽位
 * @param  rps 输出轴目标转速，有限值，超限时按配置限幅
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM /
 *         PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_NOT_SUPPORTED
 */
platform_err_t platform_pmotor_set_rps(platform_pmotor_id_t id, float rps);

/**
 * @brief  设置多个物理电机的目标转速，自动使能对应电机
 *         会校验各参数是否有效，若有无效参数则不执行任何操作
 * @param  targets 目标数组，不可为NULL
 * @param  count   目标数量，范围为1到PLATFORM_PMOTOR_ID_MAX
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM /
 *         PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_NOT_SUPPORTED
 */
platform_err_t platform_pmotor_set_targets(
    const platform_pmotor_target_t *targets,
    uint8_t count);

/**
 * @brief  读取一个物理电机的同拍状态快照
 * @param  id    稳定物理槽位
 * @param  state 输出状态，不可为NULL
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM /
 *         PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_NOT_SUPPORTED
 * @note   读取失败时不修改输出内容
 */
platform_err_t platform_pmotor_get_state(
    platform_pmotor_id_t id,
    platform_pmotor_state_t *state);

#endif /* PLATFORM_PMOTOR_H */
