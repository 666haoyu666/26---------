# 操作系统适配层（OS Platform）

## 模块定位

`02_OS_Platform` 是业务代码与 FreeRTOS 之间的唯一适配层。APP、Server、
BSP 和 Middleware 只能包含 `OS_Wrapper/inc` 中的公共 OSAL 头文件，不得直接
包含 FreeRTOS 头文件或调用 FreeRTOS API。

调用关系如下：

```text
APP / Server / BSP / Middleware
              ↓
       OS_Wrapper（OSAL）
              ↓
       Middlewares/FreeRTOS
```

当前产品固定使用 FreeRTOS，因此不再设置单独的 `OS_Implementation` 转发
层。`osal_*.c` 直接完成参数校验、上下文判断、时间换算和 FreeRTOS API
调用。

## 目录结构

```text
02_OS_Platform/
└── OS_Wrapper/
    ├── inc/
    │   ├── osal.h             公共聚合头文件
    │   ├── osal_*.h           公共 OSAL 接口
    │   └── common_types.h     OSAL 公共类型
    └── src/
        ├── osal_*.c           OSAL 的 FreeRTOS 实现
        └── osal_freertos_priv.h
                                仅供 OSAL 源文件使用
```

不要再创建 `Middlewares/os_adapter` 或 `OS_Implementation`。`Middlewares`
只保留 CubeMX 生成或第三方提供的 FreeRTOS 源码。

## 包含规则

上层按需包含公共接口：

```c
#include "osal_task.h"
#include "osal_queue.h"
```

同一文件使用多个 OSAL 模块时，也可以包含聚合头：

```c
#include "osal.h"
```

`osal_freertos_priv.h`、`FreeRTOS.h`、`task.h`、`queue.h`、`semphr.h` 和
`timers.h` 均为 OS 平台私有依赖，禁止出现在上层模块。

## 接口约定

- `osal_task_create()` 的 `stack_size` 单位为字节。
- `osal_task_delay()` 的单位为 tick。
- 其余带 `timeout`、`period` 或 `delay_ms` 的接口单位为毫秒。
- `OSAL_MAX_DELAY` 表示永久等待。
- 句柄均为不透明类型，上层不得解引用或转换为 FreeRTOS 句柄。
- ISR 中只允许调用明确支持 ISR 的队列、信号量和定时器接口。
- 互斥锁和普通临界区接口不得在 ISR 中使用。
- CubeMX 已在 `main.c` 中启动 CMSIS-RTOS2 调度器，上层不得再调用
  `osal_task_start()`。

## 工程接入

构建系统需要完成以下配置：

1. 加入 `OS_Wrapper/src/osal_*.c`。
2. 加入头文件路径 `OS_Wrapper/inc`。
3. 加入 FreeRTOS、CMSIS 的 Vendor 头文件路径和 FreeRTOS 源文件。
4. 确保 `FreeRTOSConfig.h` 启用实际使用的任务、队列、信号量和软件定时器。
5. 不得同时编译其他目录中的 `osal_*.c`，否则会产生重复符号。
