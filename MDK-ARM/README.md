# MDK-ARM（Keil 工程 / 构建）

Keil MDK 工程文件（`.uvprojx`、`.uvoptx`、`RTE/`、启动汇编等）与烧录配置。

- 选型后由 CubeMX 生成 MDK-ARM 工程，或手动创建；把上述各层目录加入工程分组与 include 路径。
- include 路径至少需含：`03_Config/`、`02_MCU_Platform/MCU_Common/`、各 `MCU_Core_XXX/`、`Platform_Interface/*`、`04_Debug_Tool/RTT/`。
- 当前为占位。若改用 CMake / IAR，可在此并列另建工程目录。
