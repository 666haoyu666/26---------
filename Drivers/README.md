# Drivers（Vendor · HAL / CMSIS）

厂家底座：`STM32xxxx_HAL_Driver/`、`CMSIS/`。**唯一存放 HAL 源码的地方**，由各 MCU 外设的 `xxx_port/` 调用。

- 属 Vendor 层，物理保留在工程根（CubeMX 重新生成，**勿移动**）。
- **不直接改源码**：必要修改走 patch 并登记 `00_Docs/vendor_mapping.md`。
- 选型 + CubeMX 后生成，当前为占位。
