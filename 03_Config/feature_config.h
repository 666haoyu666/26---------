#ifndef __FEATURE_CONFIG_H__
#define __FEATURE_CONFIG_H__

/*
 * 功能裁剪开关：用 0/1 决定某个功能是否编译进固件（关闭时应做到零开销）。
 * 循迹小车功能尚未规划，以下为示意占位，待需求确定后增删。
 */

#define FEATURE_TRACKING        0   /* 循迹感知（灰度/摄像头）        TODO */
#define FEATURE_MOTOR           0   /* 电机 / PWM 驱动                TODO */
#define FEATURE_COMM            0   /* 通信（蓝牙/串口/上位机）        TODO */
#define FEATURE_LOG             0   /* 调试日志（RTT / 串口）          TODO */
#define FEATURE_OLED            0   /* 显示 / 调参界面                TODO */

#endif /* __FEATURE_CONFIG_H__ */
