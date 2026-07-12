/**
 * @file    platform_encoder.h
 * @brief   MCU正交编码器累计计数接口
 * @note    位宽扩展、溢出处理和原子快照全部收口Impl
 */

#ifndef PLATFORM_ENCODER_H
#define PLATFORM_ENCODER_H

#include "platform_error.h"

#define PLATFORM_ENC_ID_UNUSED 0xFFFFFFFFU /* 未使用编码器 */

typedef uint32_t platform_encoder_id_t;

/**
 * @brief  启动逻辑编码器累计计数
 * @param  id 逻辑编码器标识
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM / PLATFORM_ERR_FAIL
 */
platform_err_t platform_encoder_start(platform_encoder_id_t id);

/**
 * @brief  停止逻辑编码器，保留已累计的软件扩展值
 * @param  id 逻辑编码器标识
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM / PLATFORM_ERR_FAIL
 */
platform_err_t platform_encoder_stop(platform_encoder_id_t id);

/**
 * @brief  读取不清零、不滤波的有符号累计计数
 * @param  id    逻辑编码器标识
 * @param  count 输出计数，不可为NULL
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM / PLATFORM_ERR_FAIL
 */
platform_err_t platform_encoder_get_count(
    platform_encoder_id_t id,
    int64_t *count);

#endif /* PLATFORM_ENCODER_H */
