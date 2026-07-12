#ifndef __PLATFORM_ERROR_H__
#define __PLATFORM_ERROR_H__

/*
 * 平台统一错误码（第4课·平台公共基础层）
 * 原则：所有对外接口一律返回 platform_err_t，成功 = 0，失败非 0。
 *       判断统一用 PLATFORM_IS_OK / PLATFORM_IS_ERR，不要散写 (==0)。
 * HAL 返回值映射约定（在 Impl 层做映射，不上浮到 Platform/Service）：
 *   HAL_OK -> PLATFORM_ERR_OK   HAL_BUSY -> PLATFORM_ERR_BUSY
 *   HAL_TIMEOUT -> PLATFORM_ERR_TIMEOUT   HAL_ERROR -> PLATFORM_ERR_FAIL
 */

#include "platform_type.h"

typedef enum {
    PLATFORM_ERR_OK              = 0,
    PLATFORM_ERR_GENERAL         = 1,
    PLATFORM_ERR_TIMEOUT         = 2,
    PLATFORM_ERR_PARAM           = 3,
    PLATFORM_ERR_NO_MEMORY       = 4,
    PLATFORM_ERR_NO_RESOURCE     = 5,
    PLATFORM_ERR_NOT_SUPPORTED   = 6,
    PLATFORM_ERR_NOT_INITIALIZED = 7,
    PLATFORM_ERR_ALREADY_INIT    = 8,
    PLATFORM_ERR_BUSY            = 9,
    PLATFORM_ERR_FAIL            = 10,
    PLATFORM_ERR_RESERVED        = 0x7FFFFFFF  /* 占位，保证枚举为 32 位 */
} platform_err_t;

#define PLATFORM_IS_ERR(err) ((err) != PLATFORM_ERR_OK)
#define PLATFORM_IS_OK(err)  ((err) == PLATFORM_ERR_OK)

#endif /* __PLATFORM_ERROR_H__ */
