# 05_Server 业务服务层

## 📌 模块定位
可复用、带策略的业务能力（循迹、电机编队、通信、周期任务、看门狗监控等）。每个能力一个子目录 `Server_XXX/`。

## 📁 子模块（按需新建，示例）
- `Server_Xms_Cycle/`：周期任务调度骨架
- `Server_Tracking/`：循迹感知 + 决策
- `Server_Motion/`：底盘运动 / 速度闭环编排
- `Server_WatchdogMonitor/`：任务级软件看门狗

## ✅ 命名规范
- 函数：`server_能力_操作()`，如 `server_motion_set_speed()`
- 生命周期统一按 `init → start → process → stop → deinit` 五段设计。

## 🔄 依赖关系
- 向上被 `01_APP` 调用；向下调用 `02_*_Platform` 接口。
- **禁止**：直接碰 Vendor / include HAL / 绕过 Platform 直接调 `xxx_port`。

> 说明：本层依赖层级在 `02_*_Platform` **之上**（编号 05 > 02 只是命名，不代表依赖顺序）。
