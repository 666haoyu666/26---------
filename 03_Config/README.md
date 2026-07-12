# 03_Config 配置层

## 📌 模块定位
集中所有可调开关与参数、板级映射、版本信息。App / Server 只 include 聚合入口 `app_config.h`。

## 📁 文件
| 文件 | 职责 |
|---|---|
| `app_config.h` | 应用级参数聚合入口（汇总 product/feature/compile 三份），上层只 include 它 |
| `product_config.h` | 产品级参数 |
| `feature_config.h` | 功能裁剪 0/1 开关，关闭时零开销 |
| `compile_config.h` | 编译期开关 |
| `board_types.h` | **类型链路源头**：定义 `uint8/uint16/uint32/...`，不 include 标准库。换板/换编译器的位宽差异收口于此，经 `MCU_Common/platform_type.h` 出口给上层 |
| `board_motor_map.h` | 板级电机资源映射声明（严格来源于 `00_Docs/01_资源分配表`） |

## 🔄 依赖关系
- 全局配置层，可被各层 include；本身不依赖上层。
