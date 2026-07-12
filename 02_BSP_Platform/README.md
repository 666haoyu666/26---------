# 02_BSP_Platform 硬件设备封装层

## 📌 模块定位
封装与具体硬件设备相关的驱动（电机、编码器、循迹传感器、通信模块等），通过标准接口向上提供访问能力，屏蔽器件差异。

## 📁 模块结构
```
02_BSP_Platform/
├── Platform_Interface/   ← 上层调用的接口定义（抽象）+ 接口适配
│   └── platform_pmotor/  ← platform_pmotor.h（闭环电机能力接口）
├── Bsp_Drivers/          ← 设备驱动实现（纯逻辑 + handler 装配）
│   └── Motor/            ← pwm_motor_driver.h（单电机纯逻辑 PID）、pwm_motor_handler.h（多电机装配/控制节拍）
└── Bsp_Integration/      ← 集成 BSP 所需的、来自 MCU / OS / Middleware 的接口
```

## ✅ 命名规范
- 接口：`platform_设备_操作()`，如 `platform_pmotor_set_rps()`
- 驱动/handler：`设备_driver` / `设备_handler`
- 结构体：`设备_cfg_t` / `设备_map_t`

## 🔄 依赖关系
- 向下依赖 `02_MCU_Platform` 提供的外设逻辑接口（PWM/编码器/IO/定时器）。
- 需要的 OS/Middleware 能力经 `Bsp_Integration/` 注入，**不**直接反向依赖 Server / App。
