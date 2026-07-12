# 26 电赛 · 平台化循迹小车

按**立芯 ec_s100_watch 平台化架构**（按域分平台 + 每外设"接口/实现"同域双目录）搭建的工程骨架。当前**只有骨架**：平台公共契约 + 电机域接口原型已落地，业务（循迹/运动/通信）待规划后逐层填实，Vendor 底座待选型后放入。

## 分层与依赖

> **目录编号 ≠ 依赖顺序。** 依赖单向向下，下层不知道上层存在。

```
01_APP → 05_Server → 02_BSP_Platform → 02_MCU_Platform → Vendor(Core/Drivers/Middlewares)
                                     ↘ 02_OS_Platform / 02_Middleware_Platform（同级平台）
横切：03_Config（全局配置） · 04_Common_Utils（纯算法） · 04_Debug_Tool（日志）
```

> App 不碰 HAL · Server 不碰 Vendor · 外设接口头不含芯片头文件 · 只有各 `xxx_port/` 见 HAL · Vendor 只保留厂家底座

一条信号穿层（电机示例）：
`app_main()` → `server_motion_set_speed()` → `platform_pmotor_set_*()` → `pwm_motor_handler` → `platform_pwm/encoder/io_*()` → `xxx_port` → `HAL_TIM_PWM_xxx()`

## 目录结构

| 目录 | 职责 | 状态 |
|---|---|---|
| `00_Docs/` | 工程文档（资源分配 / 架构设计 / 接口协议 / 厂商补丁登记） | 架构文档已写（部分待同步新结构） |
| `01_APP/` | 应用层：`User_Init/`（入口转发 `app_main`）、`User_Isr_handlers/` | 仅入口骨架 |
| `02_BSP_Platform/` | 设备封装：`Platform_Interface/` + `Bsp_Drivers/` + `Bsp_Integration/` | 电机域已落地，余占位 |
| `02_MCU_Platform/` | MCU 外设接口+实现：`MCU_Common/` + `MCU_Core_XXX/`(+`xxx_port/`) + `MCU_LIB/` | 公共契约 + PWM/TIM/ENC/GPIO 接口已落地 |
| `02_OS_Platform/` | FreeRTOS 的单层 OSAL Wrapper | 已接入，Wrapper 直接调用 FreeRTOS |
| `02_Middleware_Platform/` | 通用中间件/协议/库 | 占位 |
| `03_Config/` | 全局配置 + 板级类型/映射 | 已落地 |
| `04_Common_Utils/` | 纯算法（CRC/滤波/PID/环形缓冲…） | 空（待建） |
| `04_Debug_Tool/` | 日志：`RTT/`（SEGGER RTT + `cat_log.h`） | 占位（待放 RTT 后端） |
| `05_Server/` | 业务服务层（`Server_XXX/`） | 空（待业务规划） |
| `Core/` `Drivers/` `Middlewares/` | Vendor：CubeMX/HAL/CMSIS/RTOS（物理保留于工程根） | **空**（待选型 + CubeMX） |
| `MDK-ARM/` | Keil 工程 / 构建 | 占位 |

## 已落地（可直接用的基础）

- `02_MCU_Platform/MCU_Common/` — 统一类型 `platform_type.h`、错误码 `platform_error.h`、公共宏 `platform_def.h`
- `03_Config/board_types.h` — 板级类型源头（不依赖标准库）；类型链路：`board_types.h → platform_type.h →` 上层只用 `uint32_t`
- 电机域接口原型：`MCU_Core_PWM/TIM/ENC/GPIO` 的 `platform_*.h`、`Platform_Interface/platform_pmotor/platform_pmotor.h`、`Bsp_Drivers/Motor/pwm_motor_driver|handler.h`、`03_Config/board_motor_map.h`

## 下一步（循迹小车规划确定后）

1. 主控 / 传感 / 电机选型 → 填 `00_Docs/01_资源分配表`
2. 落 CubeMX 最小工程 → `Core/Drivers/Middlewares`（Vendor 底座，物理保留于工程根）+ `MDK-ARM` 工程
3. `main.c` 只保留 `app_main()` 转发（`01_APP/User_Init/`）
4. 在各 `MCU_Core_XXX/xxx_port/` 落 HAL 适配；在 `Bsp_Drivers/` 落设备驱动
5. 在 `05_Server` 沉淀能力（循迹感知、速度闭环、通信、日志）
6. 在 `01_APP` 编排循迹主流程状态机

> 参考出处：立芯 ec_s100_watch 平台化架构；飞书课程"系统第一刀分层 / 平台公共基础层 / 对象模型 / 数据模型"。
