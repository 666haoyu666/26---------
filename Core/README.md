# Core（Vendor · CubeMX 生成）

CubeMX 生成的工程内核：`main.c`、`stm32xxxx_it.c`、`stm32xxxx_hal_msp.c`、时钟/外设初始化、启动文件等。

- **逻辑上属 Vendor 层**，但物理上保留在工程根（CubeMX 会重新生成，**勿移动**）。
- `main()` 只做启动转发 → 调用 `app_main()`（见 `01_APP/User_Init/`），不写任何业务。
- 选型 + 跑一次 CubeMX 后，本目录才会有内容。当前为占位。
