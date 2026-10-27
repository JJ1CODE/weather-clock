# f407_lvgl_basic

STM32F407ZGT6 + 240×320 ST7789 SPI 屏的 LVGL v9 基础工程。由工作区中的 `f407_SPI_lvgl` 复制并精简；原工程未修改。

## 使用

在 STM32CubeIDE 中选择 **File → Import → Existing Projects into Workspace**，目录选本工程文件夹。工程名是 `f407_lvgl_basic`。执行 **Project → Build Project**，再使用 `f407_lvgl_basic Debug` 下载。

屏幕应显示居中的 `Hello LVGL`。修改界面从 `Core/Src/main.c` 的 `LCD_Task()` 开始；显示适配位于 `Core/Src/lvgl_port.c`，ST7789 驱动位于 `Drivers/spi/lcd_spi.c`。

本模板尚未配置 I²C 和触摸输入；后续可在此工程上逐步添加。当前仅包含显示所需的代码和资源。

已用 STM32CubeIDE 1.19.0 的 Debug 配置构建通过，生成 `Debug/f407_lvgl_basic.elf`；硬件显示仍需在开发板上确认。
