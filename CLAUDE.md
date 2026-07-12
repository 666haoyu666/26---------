# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概况

26 电赛·平台化循迹小车，嵌入式 C 固件工程（预期 STM32 + CubeMX/HAL，主控未选型）。目录结构对齐**立芯 ec_s100_watch 平台化架构**（按域分平台 + 每外设"接口/实现"同域双目录）。当前**只有架构骨架**：平台公共契约（`MCU_Common`）、`board_types.h`、电机域接口原型（PWM/编码器/IO/定时器/电机）已落地，业务层为空，Vendor（`Core/Drivers/Middlewares`）与构建工程（`MDK-ARM`）为空占位，**尚无任何构建系统**。工具链落地后请在此补充构建/烧录命令。

空函数 / 空 `xxx_port/` 占位是**正常且符合预期**的——先留位置，后长血肉。

## 分层架构（核心规则，改代码前必读）

**注意：目录编号 ≠ 依赖顺序。** 依赖方向如下（单向向下，下层不知道上层存在，不跨层穿透平台）：

```
01_APP → 05_Server → 02_BSP_Platform → 02_MCU_Platform → Vendor(Core/Drivers/Middlewares)
                                     ↘ 02_OS_Platform / 02_Middleware_Platform（同级平台，经 Bsp_Integration 注入）

横切（可被各层复用）：03_Config（全局配置）  04_Common_Utils（纯算法）  04_Debug_Tool（日志）
```

| 目录 | 职责 | 禁区 |
|---|---|---|
| `01_APP` | 产品业务流程/状态机，只描述"要干什么"。`User_Init/` 启动转发，`User_Isr_handlers/` ISR 路由 | include HAL、直接操作 GPIO/ADC/PWM/I2C |
| `05_Server` | 可复用带策略的业务能力（`Server_XXX/`，每能力一个子目录） | 碰 Vendor、include HAL、绕过平台直接调 `xxx_port` |
| `02_BSP_Platform` | 设备封装（电机/编码器/传感器…）。`Platform_Interface/`=接口，`Bsp_Drivers/`=驱动实现，`Bsp_Integration/`=注入的 OS/MCU/MW 能力 | 反向调 Server/App |
| `02_MCU_Platform` | MCU 外设**逻辑接口**（`MCU_Core_XXX/` 根）+ **实现**（`xxx_port/`）+ 公共契约（`MCU_Common/`） | 反向调上层；接口头绑死芯片 |
| `02_OS_Platform` | RTOS 封装：`OS_Wrapper/`（OSAL 接口）+ `OS_Implementation/`（FreeRTOS 落地） | 反向调上层 |
| `02_Middleware_Platform` | 通用中间件/协议/库 | 反向调上层 |
| `Core` / `Drivers` / `Middlewares` | Vendor：CubeMX 生成码 / HAL / CMSIS / RTOS 源码。**物理保留在工程根，勿移动**（CubeMX 会重新生成） | 不直接改源码（走 patch 并登记 `00_Docs/vendor_mapping.md`），不反向调上层 |

**接口/实现同域**：与旧的"03_Platform 只放接口、04_Impl 只放实现"不同，本架构把接口与实现**按域放在一起**——MCU 外设 = `MCU_Core_XXX/`（接口头）+ `MCU_Core_XXX/xxx_port/`（实现）；BSP 设备 = `Platform_Interface/设备/`（接口）+ `Bsp_Drivers/设备/`（实现）。

标准调用链（电机示例）：
`app_main()`（01_APP）→ `server_motion_set_speed()`（05_Server）→ `platform_pmotor_set_*()`（BSP 接口）→ `pwm_motor_handler`（BSP 驱动，持控制节拍）→ `platform_pwm/encoder/io_*()`（MCU 接口）→ `xxx_port` → `HAL_TIM_PWM_xxx()`（Drivers）

### 违规自查清单

- `01_APP/**`、`05_Server/**` 不得 include HAL
- 外设**接口头**（`MCU_Core_XXX/` 根、`Platform_Interface/**`）不得 include 芯片头文件（绑死芯片）
- 只有各外设 `xxx_port/**`、`OS_Implementation/**`、Vendor 能看到 HAL
- 平台 / Vendor 不得反向调用上层

## 关键约定

**类型链路**：上层代码一律不直接 include `<stdint.h>`。源头是 `03_Config/board_types.h`（定义 `uint8/uint32/...`，不依赖标准库），经 `02_MCU_Platform/MCU_Common/platform_type.h` 导出成 stdint 兼容命名（`uint32_t` 等）供上层使用。换板/换编译器的位宽差异收口在 `board_types.h`。（`platform_type.h` 用裸文件名 `#include "board_types.h"`，`03_Config/` 与 `MCU_Common/` 都要进 include 路径。）

**错误码**：所有对外接口一律返回 `platform_err_t`（`MCU_Common/platform_error.h`），成功 = 0；判断统一用 `PLATFORM_IS_OK` / `PLATFORM_IS_ERR`，不散写 `(== 0)`。HAL 返回值映射到 `platform_err_t` **只在各外设 `xxx_port/` 做**，不上浮：`HAL_OK→PLATFORM_ERR_OK`、`HAL_BUSY→PLATFORM_ERR_BUSY`、`HAL_TIMEOUT→PLATFORM_ERR_TIMEOUT`、`HAL_ERROR→PLATFORM_ERR_FAIL`。

**配置**：可调开关与参数集中在 `03_Config/`，App/Server 只 include 聚合入口 `app_config.h`。功能裁剪用 `feature_config.h` 的 0/1 开关，关闭时必须零开销。

**入口链路**：CubeMX 生成的 `main()`（`Core/`）只做启动转发调用 `app_main()`（`01_APP/User_Init/app_main.c`），不写任何业务。

**生命周期**：Server/对象统一按 `init → start → process → stop → deinit` 五段设计（见 `00_Docs/02_架构设计/生命周期说明.md`）。

**文档先行**：`00_Docs/` 是规范来源——改代码前先改文档。`01_资源分配表/`（引脚/总线）是各 `xxx_port/` 与 `board_motor_map.h` 板级映射的**唯一依据**；跨模块协议（传感器数据模型、日志规范）在 `03_接口协议/`。日志规划为 SEGGER RTT 彩色分级（LOGD/LOGI/LOGW/LOGE）+ 每模块宏开关，落 `04_Debug_Tool/RTT/`（`cat_log.h` + SEGGER RTT 后端）。

> 说明：`00_Docs/02_架构设计/` 部分文档仍描述旧的"03_Platform/04_Impl 分离"五层模型，与本文所述的立芯式"按域同域"结构不完全一致，待逐篇同步更新。
