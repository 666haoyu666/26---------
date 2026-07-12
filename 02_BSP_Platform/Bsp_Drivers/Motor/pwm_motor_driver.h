/**
 * @file    pwm_motor_driver.h
 * @brief   单电机测速和闭环控制的纯逻辑合同
 * @note    不访问IO、Platform MCU接口或HAL，每个实例状态独立
 */

#ifndef PWM_MOTOR_DRIVER_H
#define PWM_MOTOR_DRIVER_H

#include <stdio.h>
#include <stdint.h>

typedef enum
{
	PMOTOR_OK             = 0,      /* Motor Operation completed successfully.    */
	PMOTOR_ERROR          = 1,      /* Motor Run-time error without case matched  */
	PMOTOR_ERRORTIMEOUT   = 2,      /* Motor Operation failed with timeout        */
	PMOTOR_ERRORRESOURCE  = 3,      /* Motor Resource not available.              */
	PMOTOR_ERRORPARAMETER = 4,      /* Motor Parameter error.                     */
	PMOTOR_ERRORNOMEMORY  = 5,      /* Motor Out of memory.                       */
	PMOTOR_ERRORISR       = 6,      /* Motor Not allowed in ISR context           */
	PMOTOR_RESERVED       = 0xFF,   /* Motor Reserved                             */

}pmotor_status_t;

/** 有方向地设置PWM占空比，这里固定上限为10000 */
typedef pmotor_status_t (*pf_pmotor_set_t)(
                                         pmotor_drv_t * const self);

/** 获取有方向的编码器累计计数值 */
typedef pmotor_status_t (*pf_pmotor_get_t)(
                                         pmotor_drv_t * const self);                                         

/** driver私有配置；PID形式可实验替换，字段语义保持稳定。 */
typedef struct {
    uint32_t  ticks_per_rev;   /* 输出轴每转编码器计数 */
    float     rps_limit;       /* 目标转速绝对值上限 */
    float     pid_kp;          /* 比例系数 */
    float     pid_ki;          /* 积分系数 */
    float     pid_kd;          /* 微分系数 */
    float     lpf_alpha;       /* 测速新值权重，范围0~1 */
    int32_t   duty_limit;      /* 输出绝对值上限，不超过1000 */
    int32_t   deadzone;        /* V1输出死区，范围0~duty_limit */
} pmotor_cfg_t;

/** PID通用状态，兼容位置式或增量式实验。 */
typedef struct {
    float  err;         /* 当前误差 */
    float  prev_err;    /* 上次误差 */
    float  prev2_err;   /* 上上次误差 */
    float  integral;    /* 积分累计 */
    float  output;      /* 未经死区处理的控制输出 */
} pmotor_ctrl_t;

/** driver输出状态，不含任何硬件标识。 */
typedef struct {
    float    target_rps;   /* 当前目标转速 */
    float    rps;          /* 滤波后转速 */
    int64_t  position;     /* 仅做极性归一化的累计计数 */
    int32_t  duty;         /* 带符号输出占空比 */
} pmotor_data_t;

/** 电机状态 */
typedef enum {
    PMOTOR_UNINIT = 0,
    PMOTOR_READY,
    PMOTOR_RUNNING,
    PMOTOR_STOPPED
} pmotor_state_t;

/** 单电机driver对象，由handler静态持有。 */
typedef struct {
    const pmotor_cfg_t  *cfg;         /* 只读配置 */
    pmotor_ctrl_t       ctrl;         /* 控制器状态 */
    pmotor_data_t       data;         /* 可观测状态 */
    int64_t             prev_count;   /* 上次原始累计计数 */
    pmotor_state_t      state;        /* driver生命周期状态 */
    /** 外部需要依赖注入 */
    pf_pmotor_set_t     set_pwm;      /* 设置PWM占空比的回调 */
    pf_pmotor_get_t     get_count;    /* 获取编码器累计计数 */
} pmotor_drv_t;

/**
 * @brief  初始化单电机纯逻辑对象
 * @param  drv driver对象，不可为NULL
 * @param  cfg 只读配置，不可为NULL
 * @retval PMOTOR_OK / PMOTOR_ERRORPARAMETER / PMOTOR_ERRORRESOURCE
 */
pmotor_status_t pmotor_inst(
    pmotor_drv_t *drv,
    const pmotor_cfg_t *cfg);

/**
 * @brief  设置单电机目标转速
 * @param  drv driver对象
 * @param  rps 有限目标值，超过配置范围时限幅
 * @retval PMOTOR_OK / PMOTOR_ERRORPARAMETER / PMOTOR_ERRORRESOURCE
 */
pmotor_status_t pmotor_set_rps(pmotor_drv_t *drv, float rps);

/**
 * @brief  停止控制器并清零目标、PID状态和输出，最终表现为电机被带动滑行
 * @param  drv driver对象
 * @retval PMOTOR_OK / PMOTOR_ERRORPARAMETER / PMOTOR_ERRORRESOURCE
 */
pmotor_status_t pmotor_stop(pmotor_drv_t *drv);

/**
 * @brief  清空单电机对象并回到未初始化状态
 * @param  drv driver对象
 * @retval PMOTOR_OK / PMOTOR_ERRORPARAMETER
 */
pmotor_status_t pmotor_deinit(pmotor_drv_t *drv);

/**
 * @brief  复制driver当前数据
 * @param  drv  driver对象
 * @param  data 输出数据，不可为NULL
 * @retval PMOTOR_OK / PMOTOR_ERRORPARAMETER / PMOTOR_ERRORRESOURCE
 */
pmotor_status_t pmotor_get_data(
    const pmotor_drv_t *drv,
    pmotor_data_t *data);

#endif /* PWM_MOTOR_DRIVER_H */
