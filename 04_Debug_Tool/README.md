# 04_Debug_Tool 调试 / 日志

## 📌 模块定位
跨层调试设施。规划为 **SEGGER RTT 彩色分级日志**（LOGD/LOGI/LOGW/LOGE）+ 每模块宏开关（关闭时零开销）。

## 📁 子模块
- `RTT/`：放 SEGGER RTT 后端源码（`SEGGER_RTT.c/.h`、`SEGGER_RTT_printf.c`、`SEGGER_RTT_Conf.h`、ASM）与统一日志封装 `cat_log.h`。
  > SEGGER RTT 源码请从 SEGGER 官方 / J-Link 安装包获取后放入本目录，**勿直接拷贝其他工程的私有实现**。

## 🔗 约定
- 日志接口协议见 `00_Docs/03_接口协议/日志规范.md`。
- ISR / 高频路径打日志需谨慎（有界、无阻塞）。

## 🔄 依赖关系
- 跨层可用；实现向下依赖 RTT 后端，不反向依赖业务层。
