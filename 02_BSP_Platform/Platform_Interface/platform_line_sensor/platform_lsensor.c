/**
 * @file    platform_lsensor.c
 * @brief   平行二值灰度传感器组的BSP契约实现
 * @note    硬件读取经输入组接口注入driver，仅支持任务或主循环单消费者
 */

#include "platform_lsensor.h"

#include "board_lsensor_map.h"
#include "bsp_lsensor_driver.h"
#include "platform_def.h"
#include "platform_io_group.h"

#define LSENSOR_RAW_MASK     (0xFFU) /* 八路原始电平有效位 */
#define LSENSOR_CENTER_INDEX (3.5f)  /* 八路等距阵列中心下标 */

/** 模块生命周期状态；接口约定为单消费者，无并发写方。 */
static platform_lsensor_run_state_t s_run_state = PLATFORM_LSENSOR_OFF;

static lsensor_drv_status_t lsensor_get_map(uint8_t *raw_map);
static void lsensor_build_cfg(lsensor_drv_cfg_t *cfg);

/**
 * @brief  将MCU输入组读取适配为driver原始位图回调
 * @param  raw_map 八路原始电平位图输出
 * @retval LSENSOR_DRV_OK / LSENSOR_DRV_ERR_PARAM / LSENSOR_DRV_ERR_IO
 */
static lsensor_drv_status_t lsensor_get_map(uint8_t *raw_map)
{
    platform_err_t err;        /* MCU输入组读取结果 */
    uint32_t       bitmap;     /* MCU层返回的组电平位图 */

    if (raw_map == NULL) {
        return LSENSOR_DRV_ERR_PARAM;
    }

    err = platform_iogroup_read(BOARD_LSENSOR_IOGROUP, &bitmap);
    if (PLATFORM_IS_ERR(err) || ((bitmap & ~LSENSOR_RAW_MASK) != 0U)) {
        return LSENSOR_DRV_ERR_IO;
    }

    *raw_map = (uint8_t)bitmap;
    return LSENSOR_DRV_OK;
}

/**
 * @brief  依据板级参数组装driver配置
 * @param  cfg 待填写的driver配置
 */
static void lsensor_build_cfg(lsensor_drv_cfg_t *cfg)
{
    uint8_t i; /* 传感器槽位游标 */

    for (i = 0U; i < LSENSOR_DRV_SLOT_MAX; i++) {
        cfg->positions_mm[i] = ((float)i - LSENSOR_CENTER_INDEX) *
                               BOARD_LSENSOR_PITCH_MM;
    }

    cfg->enabled_mask = (uint8_t)BOARD_LSENSOR_ENABLED_MASK;
    cfg->active_low   = (uint8_t)BOARD_LSENSOR_ACTIVE_LOW;
    cfg->get_map      = lsensor_get_map;
}

/**
 * @brief  装配并初始化灰度传感器组
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_ALREADY_INIT / PLATFORM_ERR_FAIL
 */
platform_err_t platform_lsensor_init(void)
{
    lsensor_drv_cfg_t    cfg;    /* 本次driver装配配置 */
    lsensor_drv_status_t status; /* driver调用结果 */

    if (s_run_state == PLATFORM_LSENSOR_ACTIVE) {
        return PLATFORM_ERR_ALREADY_INIT;
    }

    if (s_run_state == PLATFORM_LSENSOR_ERROR) {
        status = lsensor_drv_deinit();
        if (status != LSENSOR_DRV_OK) {
            return PLATFORM_ERR_FAIL;
        }
    }

    lsensor_build_cfg(&cfg);
    status = lsensor_drv_init(&cfg);
    if (status != LSENSOR_DRV_OK) {
        s_run_state = PLATFORM_LSENSOR_ERROR;
        return PLATFORM_ERR_FAIL;
    }

    s_run_state = PLATFORM_LSENSOR_ACTIVE;
    return PLATFORM_ERR_OK;
}

/**
 * @brief  注销灰度传感器组并回到OFF状态
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_FAIL
 */
platform_err_t platform_lsensor_deinit(void)
{
    lsensor_drv_status_t status; /* driver注销结果 */

    if (s_run_state == PLATFORM_LSENSOR_OFF) {
        return PLATFORM_ERR_NOT_INITIALIZED;
    }

    status = lsensor_drv_deinit();
    if (status != LSENSOR_DRV_OK) {
        s_run_state = PLATFORM_LSENSOR_ERROR;
        return PLATFORM_ERR_FAIL;
    }

    s_run_state = PLATFORM_LSENSOR_OFF;
    return PLATFORM_ERR_OK;
}

/**
 * @brief  读取灰度传感器组的同拍识别结果
 * @param  state 输出快照，失败时保持原值
 * @retval PLATFORM_ERR_OK / PLATFORM_ERR_PARAM /
 *         PLATFORM_ERR_NOT_INITIALIZED / PLATFORM_ERR_FAIL
 */
platform_err_t platform_lsensor_read(platform_lsensors_state_t *state)
{
    platform_lsensors_state_t snapshot; /* 成功后一次性提交的快照 */
    lsensor_drv_result_t      result;   /* driver单拍识别结果 */
    lsensor_drv_status_t      status;   /* driver调用结果 */

    if (state == NULL) {
        return PLATFORM_ERR_PARAM;
    }
    if (s_run_state == PLATFORM_LSENSOR_OFF) {
        return PLATFORM_ERR_NOT_INITIALIZED;
    }
    if (s_run_state != PLATFORM_LSENSOR_ACTIVE) {
        return PLATFORM_ERR_FAIL;
    }

    status = lsensor_drv_read(&result);
    if (status == LSENSOR_DRV_ERR_STATE) {
        s_run_state = PLATFORM_LSENSOR_ERROR;
        return PLATFORM_ERR_NOT_INITIALIZED;
    }
    if (status != LSENSOR_DRV_OK) {
        return (status == LSENSOR_DRV_ERR_PARAM) ?
               PLATFORM_ERR_PARAM : PLATFORM_ERR_FAIL;
    }

    switch (result.track) {
        case LSENSOR_DRV_NO_LINE:
            snapshot.track_state = PLATFORM_LSENSOR_NO_LINE;
            break;

        case LSENSOR_DRV_TRACKING:
            snapshot.track_state = PLATFORM_LSENSOR_TRACKING;
            break;

        case LSENSOR_DRV_AMBIGUOUS:
            snapshot.track_state = PLATFORM_LSENSOR_AMBIGUOUS;
            break;

        default:
            s_run_state = PLATFORM_LSENSOR_ERROR;
            return PLATFORM_ERR_FAIL;
    }

    snapshot.line_offset_mm = result.offset_mm;
    snapshot.run_state      = s_run_state;
    *state = snapshot;
    return PLATFORM_ERR_OK;
}
