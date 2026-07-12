# Middlewares（Vendor · 第三方中间件）

厂家 / 第三方中间件源码：`Third_Party/FreeRTOS/`、`os_adapter/` 等。

- 属 Vendor 层，物理保留在工程根（CubeMX 重新生成，**勿移动**）。
- RTOS 内核由 `02_OS_Platform/OS_Implementation/FreeRTOS` 适配封装后向上提供。
- 选型 + CubeMX（勾选 FreeRTOS 等）后生成，当前为占位。
