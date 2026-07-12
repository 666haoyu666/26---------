# Vendor 映射表（占位）

> 记录本工程用到的厂家/第三方底座及其来源、版本，便于升级与迁移。
> 循迹小车未选型，先留表头。

| 组件 | 来源 | 版本 | 存放位置 | 是否有本地修改 | 备注 |
|---|---|---|---|---|---|
| MCU HAL |  |  | Drivers/ |  | TODO |
| CMSIS |  |  | Drivers/CMSIS |  | TODO |
| CubeMX 生成码 |  |  | Core/ |  | 物理保留，勿手改 |
| RTOS | FreeRTOS（适配目标） | 待 CubeMX 确定 | Middlewares/ |  | OSAL 已迁移，Vendor 待落地 |
| 第三方算法/库 |  |  |  |  | TODO |

> 规则：**不直接改 Vendor 源码**。必要修改通过 Impl 适配 / wrapper 隔离 / patch 记录（见 patch/）。
