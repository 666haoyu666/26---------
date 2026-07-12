/*
 * App 层总入口实现（骨架）
 *
 * 分层规则（第3课）：
 *   - App 只描述"产品要干什么"，不关心硬件怎么干。
 *   - 只能调用 Service 层；禁止 include HAL / 直接操作 GPIO、ADC、I2C、SPI、DMA。
 *
 * 循迹小车业务尚未规划，这里先留骨架。后续典型流程会是：
 *   app_main()
 *     -> service_system_init()     系统装配 / 上电自检
 *     -> service_xxx_start()       启动各能力（循迹 / 电机 / 通信 ...）
 *     -> 进入主循环 或 启动 RTOS 调度
 */

#include "app_main.h"
/* #include "service_system/service_system.h"   // TODO: Service 层就绪后接入 */

void app_main(void)
{
    /* TODO(循迹小车): 待业务规划后填入
     *   1) 系统装配 / 上电自检握手
     *   2) 启动 Service 能力（循迹感知、电机控制、通信、日志...）
     *   3) 进入主循环或启动 RTOS 调度
     */
    for (;;)
    {
        /* 骨架占位：什么都不做，保证工程可编译、入口链路成立 */
    }
}
