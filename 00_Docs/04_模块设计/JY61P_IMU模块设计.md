# JY61P IMU 模块设计

## 一、范围与阶段

本阶段只定义 JY61P 纯协议驱动头文件，作为 `platform_imu` 的具体器件
实现基础，不实现 UART 接收、缓存管理、平台接口实现或构建集成。

稳定接口仍是
`02_BSP_Platform/Platform_Interface/platform_imu/platform_imu.h`，具体器件
协议收口在
`02_BSP_Platform/Bsp_Drivers/JY61P/bsp_jy61p_driver.h`。

## 二、职责边界

- JY61P 驱动只扫描调用方提供的字节缓冲，完成找帧、校验、解码和单位
  换算；不访问 UART、HAL、RTOS，也不持有 DMA 缓冲。
- 后续 `platform_imu` 实现负责 UART 能力装配、接收缓存、生命周期、并发
  保护，以及 `jy61p_status_t` 到 `platform_err_t` 的映射。
- APP 和 Server 只依赖 `platform_imu.h`，不得包含 JY61P 驱动头文件。
- 坐标安装方向和车体坐标变换属于板级装配或上层领域适配，不写入协议
  解析驱动。

目标依赖方向：

```text
APP / Server
    -> platform_imu
        -> JY61P protocol driver
        -> MCU USART capability
            -> USART port / HAL
```

## 三、串口帧协议

JY61P 子帧固定为 11 字节。第 11 字节为前 10 字节的无符号累加和低
8 位。多字节原始量为小端有符号 16 位整数。

| 子帧 | 字节布局 | 换算 |
|---|---|---|
| 三轴角速度 `0x55 0x52` | `WxL WxH WyL WyH WzL WzH VolL VolH SUM` | `raw / 32768 * 2000 deg/s` |
| 三轴姿态角 `0x55 0x53` | `RollL RollH PitchL PitchH YawL YawH VerL VerH SUM` | `raw / 32768 * 180 deg` |

图示协议中的 `VolL/VolH` 对非蓝牙产品无效，本模块不向上暴露该字段。

## 四、驱动数据合同

`jy61p_sample_t` 保存一次扫描得到的最新数据：

| 字段 | 单位 | 映射到平台层 |
|---|---|---|
| `roll_deg` | deg | `platform_imu_data_t.roll_deg` |
| `pitch_deg` | deg | `platform_imu_data_t.pitch_deg` |
| `yaw_deg` | deg | `platform_imu_data_t.yaw_deg` |
| `wx_dps` | deg/s | `platform_imu_data_t.wx` |
| `wy_dps` | deg/s | `platform_imu_data_t.wy` |
| `wz_dps` | deg/s | `platform_imu_data_t.wz` |
| `valid_mask` | 位掩码 | 指示角速度帧和角度帧是否分别有效 |

解析器允许一个缓冲中只有一种目标子帧，并用 `valid_mask` 表达实际有效
数据。`platform_imu_read()` 只有在 `JY61P_DATA_ALL` 全部有效时才提交完整
快照；否则不得修改调用方输出。

同一缓冲存在同类多帧时取最后一帧。校验失败、未知 ID 和不完整尾帧会被
跳过；缓冲中没有任何有效角速度或角度帧时返回 `JY61P_ERR_FRAME`。

## 五、错误与输出语义

| 驱动状态 | 含义 | 平台层预期映射 |
|---|---|---|
| `JY61P_OK` | 至少解析出一种目标子帧 | 数据完整时 `PLATFORM_ERR_OK` |
| `JY61P_ERR_PARAM` | 输入、输出为空或长度为 0 | `PLATFORM_ERR_PARAM` |
| `JY61P_ERR_FRAME` | 没有有效目标子帧 | `PLATFORM_ERR_FAIL` |

`jy61p_parse()` 失败时不修改输出；成功时未置位的数据字段填 0，调用方必须
先检查 `valid_mask` 再消费字段。

## 六、后续闭环项

- 实现 `bsp_jy61p_driver.c`，并用主机测试覆盖正常帧、负数、满量程、
  错位包头、坏校验、不完整帧和同类多帧。
- 补齐 MCU USART 逻辑接口及 `usart_port`，HAL 只出现在端口实现。
- 实现 `platform_imu.c` 的接收缓存、生命周期、状态映射和一致快照。
- 按资源分配表确定 UART、DMA、波特率与中断优先级后再做板级装配。
- 将实现文件、头文件路径和配置加入实际构建目标，并完成真机验证。
