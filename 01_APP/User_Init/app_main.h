#ifndef __APP_MAIN_H__
#define __APP_MAIN_H__

/*
 * App 层总入口（第3课·五层分层）
 * 约定：main.c 只做启动转发 —— 调用 app_main() 后不再写任何业务逻辑。
 * 依赖方向：App -> Service -> Platform -> Impl -> Vendor（单向向下，禁止 include HAL）
 */

/**
 * @brief  应用总入口，由 CubeMX 生成的 main() 在外设初始化完成后调用。
 *         内部只允许调用 Service 层能力，不允许直接碰 HAL / 硬件。
 * @note   本函数不返回（内部为 RTOS 启动或主循环）。
 */
void app_main(void);

#endif /* __APP_MAIN_H__ */
