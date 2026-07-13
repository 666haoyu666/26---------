#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

/*
 * 应用级参数：任务优先级、周期、缓冲区大小等"业务可调参数"的总入口。
 * 汇总产品/功能/编译三份配置，App/Service 只需 include 本文件。
 */

#include "product_config.h"
#include "feature_config.h"
#include "compile_config.h"

#define APP_ODOM_PERIOD_MS       10U  /* 里程计解算周期，ms */

/* 示例占位（待循迹小车任务划分后填写）：
 * #define APP_TASK_TRACKING_PRIO   3
 * #define APP_TASK_MOTOR_PERIOD_MS 5
 */

#endif /* __APP_CONFIG_H__ */
