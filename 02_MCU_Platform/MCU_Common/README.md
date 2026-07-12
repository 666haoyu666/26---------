# MCU_Common 平台公共契约

整个工程的**类型链路**与**错误码**源头出口，被各层复用。

| 文件 | 职责 |
|---|---|
| `platform_type.h` | 把板级源头类型（`03_Config/board_types.h`）导出成 stdint 兼容命名（`uint32_t` 等）。上层**不**直接 include `<stdint.h>`。 |
| `platform_error.h` | 统一错误码 `platform_err_t`（成功=0）；判断用 `PLATFORM_IS_OK` / `PLATFORM_IS_ERR`。HAL→platform_err 的映射只在各外设 `xxx_port/` 做。 |
| `platform_def.h` | 公共宏（对齐、ARRAY_SIZE、延时宏）+ 通用接口声明（实现落 Impl）。 |

## ⚠️ include 路径
`platform_type.h` 用裸文件名 `#include "board_types.h"` 引入板级类型；编译器 include 目录需同时包含 `03_Config/` 与本目录 `02_MCU_Platform/MCU_Common/`。
