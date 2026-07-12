#ifndef __PRODUCT_CONFIG_H__
#define __PRODUCT_CONFIG_H__

/*
 * 产品级配置：产品型号、版本、目标板等"这是什么产品"的信息。
 * 循迹小车细节未定，先留占位。
 */

#define PRODUCT_NAME            "LineTracer"          /* 产品名 */
#define PRODUCT_MODEL           "26-DIANSAI-CAR"      /* 型号 */

#define PRODUCT_VERSION_MAJOR   0
#define PRODUCT_VERSION_MINOR   1
#define PRODUCT_VERSION_PATCH   0

/* 目标主控：待选型（STM32 / 其它），选定后在此固化，并配套 04_Impl/impl_mcu */
/* #define BOARD_MCU_STM32Fxxx */

#endif /* __PRODUCT_CONFIG_H__ */
