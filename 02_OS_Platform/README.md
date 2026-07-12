# 02_OS_Platform 操作系统封装层（OSAL）

## 📌 模块定位
封装 RTOS 内核能力，对外统一提供任务、队列、信号量、内存等接口，便于替换操作系统（裸机 / FreeRTOS 可切换）。

## 📁 模块结构
```
02_OS_Platform/
├── OS_Wrapper/          ← 统一 OSAL 接口（对上）
│   ├── inc/             ← osal_task / osal_sema / osal_queue / osal_heap 等头
│   └── src/             ← wrapper 实现
└── OS_Implementation/   ← 具体 RTOS 落地（对下）
    └── FreeRTOS/        ← FreeRTOS 适配
```

## ✅ 命名规范
- 接口：`osal_对象_操作()`，如 `osal_task_create()`、`osal_sema_take()`
- 结构体：`osal_对象_cfg_t`

## 🔄 依赖关系
- 向下依赖具体 RTOS（Vendor 的 `Middlewares/Third_Party/FreeRTOS`）。
- 向上供 App / Middleware / BSP(Integration) 调用。
