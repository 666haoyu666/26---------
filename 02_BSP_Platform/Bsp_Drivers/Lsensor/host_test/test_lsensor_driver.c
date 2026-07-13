/**
 * @file    test_lsensor_driver.c
 * @brief   bsp_lsensor_driver主机侧单测（WP3用例表）
 * @note    构建（host_test目录内执行）：
 *          gcc -std=c11 -Wall -Wextra -I.. -I../../../../02_Common_Platform
 *              -I../../../../03_Config ../bsp_lsensor_driver.c
 *              test_lsensor_driver.c -o test_lsensor
 */

#include <stdio.h>

#include "bsp_lsensor_driver.h"

#define CHECK(cond)                                          \
    do {                                                     \
        if (!(cond)) {                                       \
            (void)printf("FAIL L%d: %s\n", __LINE__, #cond); \
            g_fail++;                                        \
        }                                                    \
    } while (0)

/* 等间距d=10mm的八路位置表：(i-3.5)*d */
static const float k_pos[LSENSOR_DRV_SLOT_MAX] = {
    -35.0f, -25.0f, -15.0f, -5.0f, 5.0f, 15.0f, 25.0f, 35.0f
};

static int                  g_fail;                 /* 失败计数 */
static uint8_t              g_raw;                  /* 假源位图 */
static lsensor_drv_status_t g_ret = LSENSOR_DRV_OK; /* 假源返回码 */

static lsensor_drv_status_t fake_get(uint8_t *raw_map)
{
    if (g_ret == LSENSOR_DRV_OK) {
        *raw_map = g_raw;
    }
    return g_ret;
}

static int f_eq(float a, float b)
{
    float d = a - b; /* 差值 */

    if (d < 0.0f) {
        d = -d;
    }
    return d < 0.001f;
}

static lsensor_drv_cfg_t make_cfg(uint8_t mask, uint8_t alow)
{
    lsensor_drv_cfg_t c; /* 组装中的配置 */
    int               i; /* 槽位游标 */

    for (i = 0; i < (int)LSENSOR_DRV_SLOT_MAX; i++) {
        c.positions_mm[i] = k_pos[i];
    }
    c.enabled_mask = mask;
    c.active_low   = alow;
    c.get_map      = fake_get;
    return c;
}

/* 注入raw并断言track与偏差 */
static void read_expect(uint8_t raw, lsensor_drv_track_t tr, float off)
{
    lsensor_drv_result_t r = { -999.0f, LSENSOR_DRV_AMBIGUOUS };

    g_ret = LSENSOR_DRV_OK;
    g_raw = raw;
    CHECK(lsensor_drv_read(&r) == LSENSOR_DRV_OK);
    CHECK(r.track == tr);
    CHECK(f_eq(r.offset_mm, off));
}

int main(void)
{
    lsensor_drv_cfg_t    cfg;               /* 被测配置 */
    lsensor_drv_result_t r;                 /* 读取结果 */
    volatile float       zero = 0.0f;       /* 构造NaN用 */
    int                  i;                 /* 槽位游标 */

    /* 未初始化读取 */
    CHECK(lsensor_drv_read(&r) == LSENSOR_DRV_ERR_STATE);

    /* init参数校验 */
    CHECK(lsensor_drv_init(NULL) == LSENSOR_DRV_ERR_PARAM);
    cfg = make_cfg(0xFFU, 0U);
    cfg.get_map = NULL;
    CHECK(lsensor_drv_init(&cfg) == LSENSOR_DRV_ERR_PARAM);
    cfg = make_cfg(0x00U, 0U);
    CHECK(lsensor_drv_init(&cfg) == LSENSOR_DRV_ERR_PARAM);
    cfg = make_cfg(0xFFU, 0U);
    cfg.positions_mm[4] = -6.0f; /* 破坏严格递增 */
    CHECK(lsensor_drv_init(&cfg) == LSENSOR_DRV_ERR_PARAM);
    cfg = make_cfg(0xFFU, 0U);
    cfg.positions_mm[2] = zero / zero; /* NaN注入 */
    CHECK(lsensor_drv_init(&cfg) == LSENSOR_DRV_ERR_PARAM);
    cfg = make_cfg(0xF7U, 0U);
    cfg.positions_mm[3] = 999.0f; /* 禁用槽位坐标不参与校验 */
    CHECK(lsensor_drv_init(&cfg) == LSENSOR_DRV_OK);

    /* 全八路基础判定 */
    cfg = make_cfg(0xFFU, 0U);
    CHECK(lsensor_drv_init(&cfg) == LSENSOR_DRV_OK);
    read_expect(0x00U, LSENSOR_DRV_NO_LINE, 0.0f);
    for (i = 0; i < (int)LSENSOR_DRV_SLOT_MAX; i++) {
        read_expect((uint8_t)(1U << i), LSENSOR_DRV_TRACKING, k_pos[i]);
    }
    read_expect(0x18U, LSENSOR_DRV_TRACKING, 0.0f);   /* 槽3+4 */
    read_expect(0x1CU, LSENSOR_DRV_TRACKING, -5.0f);  /* 槽2+3+4 */
    read_expect(0x23U, LSENSOR_DRV_AMBIGUOUS, 0.0f);  /* 双簇 */
    read_expect(0x81U, LSENSOR_DRV_AMBIGUOUS, 0.0f);  /* 两端双簇 */
    read_expect(0xFFU, LSENSOR_DRV_AMBIGUOUS, 0.0f);  /* 全亮 */

    /* 禁用槽位=断点与位裁剪 */
    cfg = make_cfg(0xF7U, 0U);
    CHECK(lsensor_drv_init(&cfg) == LSENSOR_DRV_OK);
    read_expect(0x14U, LSENSOR_DRV_AMBIGUOUS, 0.0f);  /* 2|4被3断开 */
    read_expect(0x0CU, LSENSOR_DRV_TRACKING, -15.0f); /* 位3被裁剪 */

    /* 部分启用时全路触发 */
    cfg = make_cfg(0x0FU, 0U);
    CHECK(lsensor_drv_init(&cfg) == LSENSOR_DRV_OK);
    read_expect(0x0FU, LSENSOR_DRV_AMBIGUOUS, 0.0f);
    read_expect(0x07U, LSENSOR_DRV_TRACKING, -25.0f);

    /* 低有效归一 */
    cfg = make_cfg(0xFFU, 1U);
    CHECK(lsensor_drv_init(&cfg) == LSENSOR_DRV_OK);
    read_expect((uint8_t)(~0x18U), LSENSOR_DRV_TRACKING, 0.0f);
    read_expect(0xFFU, LSENSOR_DRV_NO_LINE, 0.0f);

    /* IO失败不改输出 */
    r.offset_mm = -999.0f;
    r.track     = LSENSOR_DRV_TRACKING;
    g_ret = LSENSOR_DRV_ERR_IO;
    CHECK(lsensor_drv_read(&r) == LSENSOR_DRV_ERR_IO);
    CHECK(f_eq(r.offset_mm, -999.0f));
    CHECK(r.track == LSENSOR_DRV_TRACKING);
    g_ret = LSENSOR_DRV_OK;

    /* 出参校验与生命周期 */
    CHECK(lsensor_drv_read(NULL) == LSENSOR_DRV_ERR_PARAM);
    CHECK(lsensor_drv_deinit() == LSENSOR_DRV_OK);
    CHECK(lsensor_drv_read(&r) == LSENSOR_DRV_ERR_STATE);

    /* 重复init覆盖生效 */
    cfg = make_cfg(0xFFU, 0U);
    CHECK(lsensor_drv_init(&cfg) == LSENSOR_DRV_OK);
    for (i = 0; i < (int)LSENSOR_DRV_SLOT_MAX; i++) {
        cfg.positions_mm[i] = 2.0f * k_pos[i];
    }
    CHECK(lsensor_drv_init(&cfg) == LSENSOR_DRV_OK);
    read_expect(0x01U, LSENSOR_DRV_TRACKING, -70.0f);

    if (g_fail == 0) {
        (void)printf("ALL PASS\n");
        return 0;
    }
    (void)printf("%d FAILED\n", g_fail);
    return 1;
}
