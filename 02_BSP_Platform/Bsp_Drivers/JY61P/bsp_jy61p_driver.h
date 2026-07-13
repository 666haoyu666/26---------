/**
 * @file    bsp_jy61p_driver.h
 * @brief   JY61P串口帧纯解析驱动接口
 * @note    - 不访问UART、HAL或RTOS，只解析调用方提供的字节缓冲
 *          - 角速度与姿态角有效性独立，由平台适配层组合完整快照
 */

#ifndef BSP_JY61P_DRIVER_H
#define BSP_JY61P_DRIVER_H

#include "platform_type.h"

#define JY61P_FRAME_LEN     (11U)      /* 单个子帧长度，字节 */
#define JY61P_FRAME_HEAD    (0x55U)    /* 子帧帧头 */
#define JY61P_ID_GYRO       (0x52U)    /* 三轴角速度子帧ID */
#define JY61P_ID_ANGLE      (0x53U)    /* 三轴姿态角子帧ID */
#define JY61P_GYRO_FS_DPS   (2000.0f)  /* 角速度满量程，deg/s */
#define JY61P_ANGLE_FS_DEG  (180.0f)   /* 姿态角满量程，deg */
#define JY61P_RAW_FS        (32768.0f) /* 有符号16位原始量程 */

#define JY61P_DATA_GYRO     (1U << 0U) /* 三轴角速度有效 */
#define JY61P_DATA_ANGLE    (1U << 1U) /* 三轴姿态角有效 */
#define JY61P_DATA_ALL      (JY61P_DATA_GYRO | JY61P_DATA_ANGLE)

/** 驱动状态码，仅表达协议解析结果。 */
typedef enum {
    JY61P_OK        = 0,    /* 至少解析出一种目标子帧 */
    JY61P_ERR_PARAM = 1,    /* 参数非法 */
    JY61P_ERR_FRAME = 2,    /* 缓冲中没有有效目标子帧 */
    JY61P_RESERVED  = 0xFF  /* 保留 */
} jy61p_status_t;

/** 一次扫描得到的最新姿态角与角速度。 */
typedef struct {
    float    roll_deg;     /* 绕X轴姿态角，deg */
    float    pitch_deg;    /* 绕Y轴姿态角，deg */
    float    yaw_deg;      /* 绕Z轴姿态角，deg */
    float    wx_dps;       /* X轴角速度，deg/s */
    float    wy_dps;       /* Y轴角速度，deg/s */
    float    wz_dps;       /* Z轴角速度，deg/s */
    uint8_t  valid_mask;   /* JY61P_DATA_*有效位 */
} jy61p_sample_t;

/**
 * @brief  扫描字节缓冲并解码最新角速度与姿态角子帧
 * @param  data 原始字节缓冲，不可为NULL
 * @param  len  有效字节数，必须大于0
 * @param  out  解析结果，不可为NULL
 * @retval JY61P_OK / JY61P_ERR_PARAM / JY61P_ERR_FRAME
 * @note   成功时未置位字段为0；失败时不修改out
 */
jy61p_status_t jy61p_parse(
    const uint8_t *data,
    uint16_t len,
    jy61p_sample_t *out);

#endif /* BSP_JY61P_DRIVER_H */
