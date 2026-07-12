#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

/*
 * 平台公共宏 + 通用接口声明（第4课·平台公共基础层）
 * 原则：接口只声明不实现（如 platform_delay_ms），实现落在 Impl 层。
 */

#include "platform_type.h"

/* 通用状态码 / 布尔 */
#define PLATFORM_OK     0
#define PLATFORM_ERROR  1
#define PLATFORM_TRUE   1
#define PLATFORM_FALSE  0

#ifndef PLATFORM_BOOL
typedef uint8_t platform_bool_t;
#endif

/* NULL */
#ifndef NULL
#define NULL ((void *)0)
#endif

/* 4 字节向上对齐 */
#define PLATFORM_ALIGN_SIZE 4
#define PLATFORM_ALIGN(n)   (((n) + PLATFORM_ALIGN_SIZE - 1) & ~(PLATFORM_ALIGN_SIZE - 1))

/* 数组元素个数（只能用于数组本体；数组传参退化成指针后算出来是错的） */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

/* 延时宏（实现由 Impl 层完成——接口/实现分离范例） */
#define PLATFORM_DELAY_MS(ms) platform_delay_ms(ms)
#define PLATFORM_DELAY_US(us) platform_delay_us(us)

void platform_delay_ms(uint32_t ms);   /* 仅声明，实现见 04_Impl */
void platform_delay_us(uint32_t us);   /* 仅声明，实现见 04_Impl */

#endif /* __PLATFORM_DEF_H__ */
