# WS2812 Demo

## 简介

WS2812 Demo 默认采用 SPI+DMA 的驱动方式，使用 `GPIO12` 作为WS2812的信号引脚。并且同时支持 IR 模拟方式进行驱动。

## 切换成IR驱动方式

> IR 驱动仅支持 GPIO11，并且需要一个1K 上拉电阻，否则无法正常工作。

- 打开 [proj_config.mk](proj_config.mk) 文件
- 将 `CONFIG_WS2812_MODE:=SPI_MODE` 设置为 `CONFIG_WS2812_MODE:=IR_MODE`。
- 重新编译并烧录程序。

```
make flash -j p=/dev/ttyUSBx
```

