# Smart-Screen
一个基于南京沁恒公司的无线型青稞RISC-V MCU: CH32V208WBU6 开发的智能点阵屏。点阵屏能够通过低功耗蓝牙（BLE）从手机/PC 接收显示信息，支持四种滚动显示模式，颜色修改，具有本地存储功能。汉字滚动显示方式可以通过手机和本地按键设置。

---

## 硬件平台

这里列出了本实验可能需要用到的一部分硬件。

### MCU

南京沁恒公司的无线型青稞RISC-V MCU: CH32V208WBU6，其结构框图如下

![image](https://github.com/Sen-Yao/Smart-Screen/assets/62921296/42a52001-28bb-4b5a-b38d-648bcdc0a2a5)

板载 BLE 5.3 模块，可用于数据通信。板载 160KB FLASH 和 32 KB SRAM。

### 显示屏

1.8 寸 RGB 彩色 TFT 液晶 LCD 显示屏，分辨率 160*128，接口为 SPI 协议。工作电压 3.3 V，驱动芯片 ST7735

### 下载调试器

为了将程序从 PC 上烧录进 MCU，需要使用下载调试器将 PC 和 MCU 串口连接。这里使用的是 WCHLink。

### 其他

- 供电，如 5V 适配器
- 杜邦线若干
- 可能需要电烙铁进行焊接

## 开发平台

本项目大致可以分为 MCU 部分和 iOS 部分，两者相互独立，可以同时开发。

MCU 采用沁恒公司提供的 IDE MountRiver Studio 进行开发，http://www.mounriver.com/，语言为 C 语言

iOS App 采用 Mac 平台上的 Xcode 使用 swift 语言进行开发。

其他信息参照两个文件夹。
