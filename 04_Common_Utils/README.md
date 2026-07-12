# 04_Common_Utils 通用工具 / 纯算法

## 📌 模块定位
与业务、硬件无关的纯算法工具：CRC、环形缓冲、滤波、PID 数学、坐标/循迹算法等。不依赖任何上层，可被各层复用。

## 📁 子模块（按需新建）
- `Filter/`：均值 / 低通 / 卡尔曼
- `PID/`：通用 PID（若从电机 driver 抽离）
- `Ring_Buffer/`：环形缓冲
- `CRC/`：校验

## 🔄 依赖关系
- **纯函数**，不 include HAL、不依赖 Platform / Server / App。便于脱离硬件单元测试。
