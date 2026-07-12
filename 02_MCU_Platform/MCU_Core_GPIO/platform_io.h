/**
 * @file    platform_io.h
 * @brief   MCU逻辑IO输出接口
 * @note    逻辑IO到端口和引脚的映射只存在于Impl
 */

#ifndef PLATFORM_IO_H
#define PLATFORM_IO_H

#include "platform_error.h"

#define PLATFORM_IO_ID_UNUSED 0xFFFFFFFFU /* 未使用逻辑IO */

typedef uint32_t platform_io_id_t;

typedef enum {
    PLATFORM_IO_LOW = 0,
    PLATFORM_IO_HIGH
} platform_io_level_t;

/**
 * @brief  写逻辑IO输出电平
 * @param  id    逻辑IO标识
 * @param  level 输出电平
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM / PLATFORM_ERR_FAIL
 */
platform_err_t platform_io_write(
    platform_io_id_t id,
    platform_io_level_t level);

#endif /* PLATFORM_IO_H */
