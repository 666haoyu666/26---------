#ifndef __PLATFORM_TYPE_H__
#define __PLATFORM_TYPE_H__

/*
 * 平台类型出口（平台公共契约层 · 02_MCU_Platform/MCU_Common）
 * 职责：把板级源头类型(board_types.h)导出成 stdint 兼容命名，供上层统一使用。
 * 核心思想：uint32_t 不直接来自 <stdint.h>，而是从板级源头经平台出口暴露。
 * 类型链路：03_Config/board_types.h(uint8/..) -> 本文件(uint8_t/..) -> 上层只用 uint32_t
 * 注意：board_types.h 现位于 03_Config/，此处用裸文件名引入，靠工程 include 路径解析
 *       （03_Config 与 02_MCU_Platform/MCU_Common 均需加入编译器 include 目录）。
 */

#include "board_types.h"

/* 平台基础类型（兼容 stdint 命名） */
typedef int8   int8_t;   typedef uint8  uint8_t;
typedef int16  int16_t;  typedef uint16 uint16_t;
typedef int32  int32_t;  typedef uint32 uint32_t;
typedef int64  int64_t;  typedef uint64 uint64_t;

typedef float  float_t;  typedef double double_t;
typedef char   char_t;   typedef uint8  uchar_t;
typedef uint8  bool_t;

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif /* __PLATFORM_TYPE_H__ */
