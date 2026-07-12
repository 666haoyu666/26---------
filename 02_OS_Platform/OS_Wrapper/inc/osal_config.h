#ifndef OSAL_CONFIG_H
#define OSAL_CONFIG_H

#define FREERTOS_SUPPORT       (1U)
#define ZEPHYR_SUPPORT         (2U)

#define OSAL_RTOS_SUPPORT      (FREERTOS_SUPPORT)
#define OSAL_USE_16_BIT_TICKS  (0U)
#define OSAL_NAME_MAX_LEN      (16U)

#endif /* OSAL_CONFIG_H */
