/**
 * @file    Sever_diff_dri_odom.c
 * @brief   差速底盘里程计服务接口
 * @note    基于左右轮原始累计计数解算平面位姿与底盘速度
 */

#include <math.h>

#include "Sever_diff_dri_odom.h"

#include "app_config.h"
#include "platform_def.h"
#include "osal_mutex.h"
#include "osal_task.h"

#define ODOM_YAW_PERIOD_DEG  360.0F  /* 航向角周期，deg */
#define ODOM_YAW_HALF_DEG    180.0F  /* 航向角半周期，deg */
#define ODOM_PI_RAD           3.14159265358979323846F /* 圆周率 */
#define ODOM_RAD_PER_DEG      (ODOM_PI_RAD / 180.0F)  /* deg转rad */
#define ODOM_DEG_PER_RAD      (180.0F / ODOM_PI_RAD)  /* rad转deg */
#define ODOM_MS_PER_S         1000.0F                  /* 每秒毫秒数 */
#define ODOM_I32_MAX          2147483647               /* int32上界 */
#define ODOM_I32_MIN          (-ODOM_I32_MAX - 1)      /* int32下界 */
#define ODOM_I32_MAX_F        2147483520.0F /* float安全转换上界 */
#define ODOM_I32_MIN_F        (-2147483648.0F) /* float安全转换下界 */
#define ODOM_U32_MAX          0xFFFFFFFFU /* uint32上界 */

static server_odom_cfg_t       g_odom_cfg;    /* 里程计配置 */
static osal_task_handle_t      g_odom_task;   /* 里程计任务句柄 */
static osal_mutex_handle_t     g_odom_mutex;  /* 里程计状态互斥锁 */
static server_odom_state_t     g_odom_state;  /* 里程计状态快照 */
static float                   g_odom_x_mm;   /* X方向浮点累计位置 */
static float                   g_odom_y_mm;   /* Y方向浮点累计位置 */
static uint32_t                g_pose_epoch;  /* 位姿校准代次 */

static float odom_norm_yaw(float yaw_deg)
{
    float norm_deg; /* 归一化后的航向角 */

    norm_deg = fmodf(yaw_deg + ODOM_YAW_HALF_DEG,
                     ODOM_YAW_PERIOD_DEG);
    if (norm_deg < 0.0F) {
        norm_deg += ODOM_YAW_PERIOD_DEG;
    }

    return norm_deg - ODOM_YAW_HALF_DEG;
}

/**
 * @brief  将有限浮点数四舍五入并饱和为int32
 * @param  value 待转换数值
 * @retval 转换后的int32数值
 */
static int32_t odom_round_i32(float value)
{
    if (!isfinite(value)) {
        return 0;
    }
    if (value > ODOM_I32_MAX_F) {
        return ODOM_I32_MAX;
    }
    if (value <= ODOM_I32_MIN_F) {
        return ODOM_I32_MIN;
    }

    if (value >= 0.0F) {
        return (int32_t)(value + 0.5F);
    }
    return (int32_t)(value - 0.5F);
}

/**
 * @brief  按二进制补码模运算计算累计tick差值
 * @param  now  当前累计tick
 * @param  last 上次累计tick
 * @retval 当前值相对上次值的有符号增量
 */
static int64_t odom_tick_delta(int64_t now, int64_t last)
{
    uint64_t diff;    /* 无符号模减结果 */
    uint64_t u64_max; /* uint64最大值 */
    uint64_t i64_max; /* int64正数最大值 */

    u64_max = ~(uint64_t)0;
    i64_max = u64_max >> 1;
    diff = (uint64_t)now - (uint64_t)last;

    if (diff <= i64_max) {
        return (int64_t)diff;
    }
    return -1 - (int64_t)(u64_max - diff);
}

static void odom_wait_period(osal_tick_type_t *last_wake)
{
    platform_err_t err; /* OSAL定周期延时状态 */

    err = osal_task_wait_until(last_wake, APP_ODOM_PERIOD_MS);
    if (PLATFORM_IS_ERR(err)) {
        osal_task_suspend(g_odom_task);
        *last_wake = osal_task_get_tick_count();
    }
}

/**
 * @brief  原子发布运行模式并清零瞬时速度
 * @param  mode 待发布的运行模式
 * @retval PLATFORM_ERR_OK或OSAL Mutex错误码
 */
static platform_err_t odom_set_mode(server_odom_mode_t mode)
{
    platform_err_t err; /* OSAL接口返回状态 */

    err = osal_mutex_take(g_odom_mutex, OSAL_MAX_DELAY);
    if (PLATFORM_IS_ERR(err)) {
        return err;
    }

    g_odom_state.vx_mm_s = 0;
    g_odom_state.vy_mm_s = 0;
    g_odom_state.w_deg_s = 0.0F;
    g_odom_state.mode = mode;

    return osal_mutex_give(g_odom_mutex);
}

/**
 * @brief  读取当前位姿校准代次
 * @param  epoch 输出校准代次，不可为NULL
 * @retval PLATFORM_ERR_OK或OSAL Mutex错误码
 */
static platform_err_t odom_get_epoch(uint32_t *epoch)
{
    platform_err_t err; /* OSAL接口返回状态 */

    if (epoch == NULL) {
        return PLATFORM_ERR_PARAM;
    }

    err = osal_mutex_take(g_odom_mutex, OSAL_MAX_DELAY);
    if (PLATFORM_IS_ERR(err)) {
        return err;
    }

    *epoch = g_pose_epoch;
    return osal_mutex_give(g_odom_mutex);
}

/**
 * @brief  基于锁内最新姿态提交一个差速里程增量
 * @param  move_mm    本周期底盘中心位移，mm
 * @param  yaw_rad    本周期顺时针航向增量，rad
 * @param  elapsed_ms 距上次有效样本的周期时长，ms
 * @param  pose_epoch 采样对应的位姿校准代次
 * @retval PLATFORM_ERR_OK或解算、OSAL Mutex错误码
 */
static platform_err_t odom_commit_step(float move_mm,
                                       float yaw_rad,
                                       uint32_t elapsed_ms,
                                       uint32_t pose_epoch)
{
    float yaw_mid; /* 本周期中点航向，rad */
    float next_x;  /* 本周期后的X浮点位置 */
    float next_y;  /* 本周期后的Y浮点位置 */
    float yaw_deg; /* 本周期后的归一化航向 */
    float scale;   /* 位移到每秒速度的换算系数 */
    float vx_mm_s; /* 本周期底盘前向速度 */
    float w_deg_s; /* 本周期顺时针角速度 */
    platform_err_t err; /* OSAL接口返回状态 */

    if (elapsed_ms == 0U) {
        return PLATFORM_ERR_PARAM;
    }

    err = osal_mutex_take(g_odom_mutex, OSAL_MAX_DELAY);
    if (PLATFORM_IS_ERR(err)) {
        return err;
    }
    if (g_pose_epoch != pose_epoch) {
        err = osal_mutex_give(g_odom_mutex);
        if (PLATFORM_IS_ERR(err)) {
            return err;
        }
        return PLATFORM_ERR_BUSY;
    }

    yaw_mid = g_odom_state.yaw_deg * ODOM_RAD_PER_DEG +
              yaw_rad * 0.5F;
    next_x = g_odom_x_mm + move_mm * cosf(yaw_mid);
    next_y = g_odom_y_mm + move_mm * sinf(yaw_mid);
    yaw_deg = odom_norm_yaw(g_odom_state.yaw_deg +
                            yaw_rad * ODOM_DEG_PER_RAD);
    scale = ODOM_MS_PER_S / (float)elapsed_ms;
    vx_mm_s = move_mm * scale;
    w_deg_s = yaw_rad * ODOM_DEG_PER_RAD * scale;

    if (!isfinite(next_x) || !isfinite(next_y) ||
        !isfinite(yaw_deg) || !isfinite(vx_mm_s) ||
        !isfinite(w_deg_s)) {
        g_odom_state.vx_mm_s = 0;
        g_odom_state.vy_mm_s = 0;
        g_odom_state.w_deg_s = 0.0F;
        g_odom_state.mode = PLATFORM_ODOM_ERROR;
        err = osal_mutex_give(g_odom_mutex);
        if (PLATFORM_IS_ERR(err)) {
            return err;
        }
        return PLATFORM_ERR_FAIL;
    }

    g_odom_x_mm = next_x;
    g_odom_y_mm = next_y;
    g_odom_state.x_mm = odom_round_i32(next_x);
    g_odom_state.y_mm = odom_round_i32(next_y);
    g_odom_state.vx_mm_s = odom_round_i32(vx_mm_s);
    g_odom_state.vy_mm_s = 0;
    g_odom_state.yaw_deg = yaw_deg;
    g_odom_state.w_deg_s = w_deg_s;
    g_odom_state.mode = PLATFORM_ODOM_ACTIVE;

    return osal_mutex_give(g_odom_mutex);
}

/** 将里程计标记为错误；Mutex异常时由任务下一周期重试。 */
static void odom_mark_error(void)
{
    platform_err_t err; /* 状态发布结果 */

    err = odom_set_mode(PLATFORM_ODOM_ERROR);
    if (PLATFORM_IS_ERR(err)) {
        return;
    }
}

/**
 * @brief  周期读取双轮累计tick并更新里程计状态。
 *
 * 每个周期分5个阶段推进，任一阶段失败即等待下一周期重来：
 *   1 计时累加      —— dt累积，跳周期不丢时间
 *   2 采样          —— 记录epoch后读取双轮tick
 *   3 基线建立/重建 —— 首次或epoch换代，只存起点不积分
 *   4 运动学解算    —— tick增量 → 中心位移与航向增量
 *   5 提交与推进    —— commit锁内校验epoch，成功才滚动基线
 */
static void server_odom_task(void *argument)
{
    platform_pmotor_state_t left_now = {0};  /* 当前左轮状态 */
    platform_pmotor_state_t right_now = {0}; /* 当前右轮状态 */
    int64_t last_left = 0;  /* 上次有效左轮累计tick */
    int64_t last_right = 0; /* 上次有效右轮累计tick */
    int64_t delta_left;     /* 本周期左轮tick增量 */
    int64_t delta_right;    /* 本周期右轮tick增量 */
    float left_mm;          /* 本周期左轮位移，mm */
    float right_mm;         /* 本周期右轮位移，mm */
    float move_mm;          /* 本周期底盘中心位移，mm */
    float yaw_rad;          /* 本周期顺时针航向增量，rad */
    uint32_t elapsed_ms = 0U; /* 距上次有效样本的周期时长 */
    uint32_t base_epoch = 0U; /* 当前差分基线的位姿代次 */
    uint32_t start_epoch;     /* 双轮采样开始前的位姿代次 */
    uint8_t have_base = 0U;   /* 左右轮差分基线有效标志 */
    osal_tick_type_t last_wake; /* 上次周期唤醒tick */
    platform_err_t err;         /* 平台接口返回状态 */

    (void)argument;
    last_wake = osal_task_get_tick_count();

    while (1) {
        /* ---- 阶段1：计时累加 ----------------------------------------
         * 上次基线有效时，把本周期时长累加进dt；上一周期若因异常/重置跳过了
         * 提交，dt会继续累积，保证速度估计的分母是真实间隔而非固定周期。
         * 无基线时dt保持0，由阶段3建立基线时统一清零。
         */
        if (have_base != 0U) {
            if (elapsed_ms <= ODOM_U32_MAX - APP_ODOM_PERIOD_MS) {
                elapsed_ms += APP_ODOM_PERIOD_MS;
            } else {
                elapsed_ms = ODOM_U32_MAX;
            }
        }

        /* ---- 阶段2：采样 --------------------------------------------
         * 采样前记录位姿代次(epoch)。采样到提交之间若有外部reset
         * 插入，由commit在锁内比对epoch兜底(BUSY)，此处不再复核。
         */
        /* 2.1 采样前记录epoch，供基线换代判断与commit校验 */
        err = odom_get_epoch(&start_epoch);
        if (PLATFORM_IS_ERR(err)) {
            odom_wait_period(&last_wake);
            continue;
        }

        /* 2.2 读取左右轮累计tick；任一轮失败即判里程计故障 */
        err = platform_pmotor_get_state(g_odom_cfg.left_id, &left_now);
        if (PLATFORM_IS_OK(err)) {
            err = platform_pmotor_get_state(g_odom_cfg.right_id,
                                             &right_now);
        }
        if (PLATFORM_IS_ERR(err)) {
            odom_mark_error();
            odom_wait_period(&last_wake);
            continue;
        }

        /* ---- 阶段3：基线建立/重建 -----------------------------------
         * 首次进入或epoch换代（外部reset过）时，本周期不做积分，
         * 只把当前双轮读数作为差分起点存下来，并把状态置为ACTIVE。
         */
        if (have_base == 0U || start_epoch != base_epoch) {
            err = odom_set_mode(PLATFORM_ODOM_ACTIVE);
            if (PLATFORM_IS_ERR(err)) {
                odom_wait_period(&last_wake);
                continue;
            }
            last_left = left_now.position;
            last_right = right_now.position;
            elapsed_ms = 0U;
            base_epoch = start_epoch;
            have_base = 1U;
            odom_wait_period(&last_wake);
            continue;
        }

        /* ---- 阶段4：差速运动学解算 ----------------------------------
         * tick增量 → 单轮位移(mm) → 底盘中心位移与航向增量：
         * 中心位移取左右轮均值，航向增量按差速模型除以轮距。
         * 结果非有限（配置异常/溢出）时判故障，不污染位姿。
         */
        delta_left = odom_tick_delta(left_now.position, last_left);
        delta_right = odom_tick_delta(right_now.position, last_right);
        left_mm = (float)delta_left * (float)g_odom_cfg.left_sign *
                  g_odom_cfg.left_mm_tick;
        right_mm = (float)delta_right * (float)g_odom_cfg.right_sign *
                   g_odom_cfg.right_mm_tick;
        move_mm = (left_mm + right_mm) * 0.5F;
        yaw_rad = (left_mm - right_mm) / g_odom_cfg.track_mm;

        if (!isfinite(left_mm) || !isfinite(right_mm) ||
            !isfinite(move_mm) || !isfinite(yaw_rad)) {
            odom_mark_error();
            odom_wait_period(&last_wake);
            continue;
        }

        /* ---- 阶段5：提交与推进基线 ----------------------------------
         * 带start_epoch提交，由commit在持锁状态下校验epoch：
         * 返回BUSY表示采样到提交之间发生了reset，本次积分作废
         * 并重建基线；其他错误保留基线，等下一周期重试（dt继续累加）。
         * 提交成功才把本次读数滚动为新基线并清零dt。
         */
        err = odom_commit_step(move_mm, yaw_rad, elapsed_ms,
                               start_epoch);
        if (err == PLATFORM_ERR_BUSY) {
            elapsed_ms = 0U;
            have_base = 0U;
            odom_wait_period(&last_wake);
            continue;
        }
        if (PLATFORM_IS_ERR(err)) {
            odom_wait_period(&last_wake);
            continue;
        }

        last_left = left_now.position;
        last_right = right_now.position;
        elapsed_ms = 0U;
        odom_wait_period(&last_wake);
    }
}

platform_err_t server_odom_init(const server_odom_cfg_t *cfg)
{
    platform_err_t err; /* OSAL接口返回状态 */

    if (cfg == NULL) {
        return PLATFORM_ERR_PARAM;
    }
    if ((uint32_t)cfg->left_id >= (uint32_t)PLATFORM_PMOTOR_ID_MAX ||
        (uint32_t)cfg->right_id >= (uint32_t)PLATFORM_PMOTOR_ID_MAX ||
        cfg->left_id == cfg->right_id) {
        return PLATFORM_ERR_PARAM;
    }
    if (!isfinite(cfg->left_mm_tick) ||
        !isfinite(cfg->right_mm_tick) ||
        !isfinite(cfg->track_mm) ||
        cfg->left_mm_tick <= 0.0F ||
        cfg->right_mm_tick <= 0.0F ||
        cfg->track_mm <= 0.0F) {
        return PLATFORM_ERR_PARAM;
    }
    if ((cfg->left_sign != -1 && cfg->left_sign != 1) ||
        (cfg->right_sign != -1 && cfg->right_sign != 1)) {
        return PLATFORM_ERR_PARAM;
    }
    if (g_odom_mutex != NULL || g_odom_task != NULL) {
        return PLATFORM_ERR_ALREADY_INIT;
    }

    err = osal_mutex_create(&g_odom_mutex);
    if (PLATFORM_IS_ERR(err)) {
        return err;
    }

    g_odom_cfg = *cfg;
    g_odom_state = (server_odom_state_t){0};
    g_odom_x_mm = 0.0F;
    g_odom_y_mm = 0.0F;
    g_pose_epoch = 0U;

    err = osal_task_create("odom_task", server_odom_task,
                           SERVER_ODOM_STACK_SIZE, SERVER_ODOM_TASK_PRIO,
                           &g_odom_task, NULL);

    /* 任务创建失败时回收已经创建的Mutex。 */
    if (PLATFORM_IS_ERR(err)) {
        g_odom_cfg = (server_odom_cfg_t){PLATFORM_PMOTOR_0};
        g_odom_x_mm = 0.0F;
        g_odom_y_mm = 0.0F;
        g_pose_epoch = 0U;
        osal_mutex_delete(g_odom_mutex);
        g_odom_mutex = NULL;
        return err;
    }

    return PLATFORM_ERR_OK;
}

platform_err_t server_odom_deinit(void)
{
    platform_err_t err; /* OSAL接口返回状态 */

    if (g_odom_mutex == NULL || g_odom_task == NULL) {
        return PLATFORM_ERR_NOT_INITIALIZED;
    }
    if (osal_task_get_current_handle() == g_odom_task) {
        return PLATFORM_ERR_BUSY;
    }

    err = osal_mutex_take(g_odom_mutex, OSAL_MAX_DELAY);
    if (PLATFORM_IS_ERR(err)) {
        return err;
    }

    osal_task_delete(g_odom_task);
    g_odom_task = NULL;
    g_odom_cfg = (server_odom_cfg_t){PLATFORM_PMOTOR_0};
    g_odom_state = (server_odom_state_t){0};
    g_odom_x_mm = 0.0F;
    g_odom_y_mm = 0.0F;
    g_pose_epoch = 0U;

    err = osal_mutex_give(g_odom_mutex);
    osal_mutex_delete(g_odom_mutex);
    g_odom_mutex = NULL;

    return err;
}

platform_err_t server_odom_set_pose(const server_odom_pose_t *pose)
{
    float yaw_deg;       /* 归一化后的航向角 */
    platform_err_t err; /* OSAL接口返回状态 */

    if (pose == NULL) {
        return PLATFORM_ERR_PARAM;
    }
    if (!isfinite(pose->yaw_deg)) {
        return PLATFORM_ERR_PARAM;
    }
    if (g_odom_task == NULL || g_odom_mutex == NULL) {
        return PLATFORM_ERR_NOT_INITIALIZED;
    }

    yaw_deg = odom_norm_yaw(pose->yaw_deg);

    err = osal_mutex_take(g_odom_mutex, OSAL_MAX_DELAY);
    if (PLATFORM_IS_ERR(err)) {
        return err;
    }
    if (g_odom_task == NULL) {
        err = osal_mutex_give(g_odom_mutex);
        if (PLATFORM_IS_ERR(err)) {
            return err;
        }
        return PLATFORM_ERR_NOT_INITIALIZED;
    }

    g_odom_state.x_mm = pose->x_mm;
    g_odom_state.y_mm = pose->y_mm;
    g_odom_x_mm = (float)pose->x_mm;
    g_odom_y_mm = (float)pose->y_mm;
    g_odom_state.yaw_deg = yaw_deg;
    g_odom_state.vx_mm_s = 0;
    g_odom_state.vy_mm_s = 0;
    g_odom_state.w_deg_s = 0.0F;
    g_pose_epoch++;

    return osal_mutex_give(g_odom_mutex);
}

platform_err_t server_odom_get(server_odom_state_t *state)
{
    server_odom_state_t snapshot; /* 同一解算时刻的状态快照 */
    platform_err_t err;           /* OSAL接口返回状态 */

    if (state == NULL) {
        return PLATFORM_ERR_PARAM;
    }
    if (g_odom_task == NULL || g_odom_mutex == NULL) {
        return PLATFORM_ERR_NOT_INITIALIZED;
    }

    err = osal_mutex_take(g_odom_mutex, OSAL_MAX_DELAY);
    if (PLATFORM_IS_ERR(err)) {
        return err;
    }
    if (g_odom_task == NULL) {
        err = osal_mutex_give(g_odom_mutex);
        if (PLATFORM_IS_ERR(err)) {
            return err;
        }
        return PLATFORM_ERR_NOT_INITIALIZED;
    }

    snapshot = g_odom_state;
    err = osal_mutex_give(g_odom_mutex);
    if (PLATFORM_IS_ERR(err)) {
        return err;
    }

    *state = snapshot;
    return PLATFORM_ERR_OK;
}
