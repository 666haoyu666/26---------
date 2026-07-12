# 01_APP 应用层

## 📌 模块定位
产品业务流程 / 状态机，只描述"要干什么"。循迹小车的上电流程、循迹状态机、任务编排在这里。

## 📁 子模块
- `User_Init/`：启动初始化与总入口。CubeMX 生成的 `main()` 只做启动转发 → 调用 `app_main()`（`User_Init/app_main.c`），此处不写业务。
- `User_Isr_handlers/`：应用级中断回调路由（把 ISR 事件分发给对应能力，禁止在 ISR 里写重逻辑）。

## 🔄 依赖关系
- 只能向下调用 `05_Server`（业务服务层）。
- **禁止**：include HAL、直接操作 GPIO/ADC/PWM/I2C/DMA、绕过 Server 直接调平台。

## 🔗 标准调用链（电机为例）
`app_main()` → `server_xxx()` → `platform_pmotor_set_*()` → `pwm_motor_handler` → `platform_pwm/encoder/io` → HAL
