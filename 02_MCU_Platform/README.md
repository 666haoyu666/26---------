# 02_MCU_Platform MCU 平台抽象层

## 📌 模块定位
封装与芯片架构 / 外设相关的能力，对上提供**稳定的逻辑外设接口**，把 TIM/CH、端口引脚等映射收口在各外设的 `xxx_port/`（实现层）里。

## 📁 子模块
```
02_MCU_Platform/
├── MCU_Common/        ← 平台公共契约：类型出口 platform_type.h、错误码 platform_error.h、公共宏 platform_def.h
├── MCU_Core_PWM/      ← platform_pwm.h  + pwm_port/（Impl）
├── MCU_Core_TIM/      ← platform_timer.h + tim_port/
├── MCU_Core_ENC/      ← platform_encoder.h + enc_port/（正交编码器计数）
├── MCU_Core_GPIO/     ← platform_io.h   + gpio_port/
├── MCU_Core_ADC/      ← adc_port/（骨架）
├── MCU_Core_IIC/      ← i2c_port/（骨架）
├── MCU_Core_SPI/      ← spi_port/（骨架）
├── MCU_Core_USART/    ← usart_port/（骨架）
└── MCU_LIB/           ← 时钟/超频等芯片级库（骨架）
```
约定：每个外设 = `接口头（MCU_Core_XXX/ 根）` + `实现端口（xxx_port/，唯一能见 HAL 的地方）`。

## 🔄 依赖关系
- 不依赖 OS、不依赖 BSP。
- 向上给 BSP / OS / Middleware / App 使用；向下 `xxx_port/` 落到 Vendor 的 HAL（`Drivers/`）。
