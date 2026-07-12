#ifndef __BOARD_TYPES_H__
#define __BOARD_TYPES_H__

/*
 * 板级基础类型源头（第4课·平台公共基础层）
 * 原则：这是整个工程"类型链路"的源头，不 include 任何标准库（不用 <stdint.h>）。
 *       换板 / 换编译器时，位宽差异收口在这里，不扩散到上层。
 * 类型链路：board_types.h(uint8/uint16/uint32) -> platform_type.h(uint8_t/...) -> 上层只用 uint32_t
 */

/* 有符号 / 无符号整型 */
typedef signed char    int8;
typedef unsigned char  uint8;
typedef signed short   int16;
typedef unsigned short uint16;
typedef signed int     int32;
typedef unsigned int   uint32;
typedef signed long    int64;   /* TODO: 选定芯片/编译器后确认 long 位宽，必要时改 long long */
typedef unsigned long  uint64;

/* 浮点 */
typedef float          float32;
typedef double         float64;

/* 布尔 */
typedef uint8          bool;
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif /* __BOARD_TYPES_H__ */
