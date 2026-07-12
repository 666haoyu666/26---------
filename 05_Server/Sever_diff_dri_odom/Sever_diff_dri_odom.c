/**
 * @file    Sever_diff_dri_odom.h
 * @brief   差速底盘里程计服务接口
 * @note    基于左右轮原始累计计数解算平面位姿与底盘速度
 */

#include "Sever_diff_dri_odom.h"
#include "platform_def.h"

static server_odom_cfg_t g_odom_cfg;

platform_err_t server_odom_init(const server_odom_cfg_t *cfg)
{
    if (cfg == NULL) {
        return PLATFORM_ERR_PARAM;
    }
    if (cfg->left_mm_tick <= 0.0f || cfg->right_mm_tick <= 0.0f || cfg->track_mm <= 0.0f) {
        return PLATFORM_ERR_PARAM;
    }
    if ((cfg->left_sign != -1 && cfg->left_sign != 1) ||
        (cfg->right_sign != -1 && cfg->right_sign != 1)) {
        return PLATFORM_ERR_PARAM;
    }

    // 初始化逻辑，设置里程计配置和状态
    g_odom_cfg.left_id = cfg->left_id;
    g_odom_cfg.right_id = cfg->right_id;

    g_odom_cfg.left_mm_tick = cfg->left_mm_tick;
    g_odom_cfg.right_mm_tick = cfg->right_mm_tick;

    g_odom_cfg.track_mm = cfg->track_mm;
    g_odom_cfg.left_sign = cfg->left_sign;
    g_odom_cfg.right_sign = cfg->right_sign;

    return PLATFORM_ERR_OK;
}


