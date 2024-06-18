# iOS

本文件夹是 iOS 相关的代码和资源，以及本项目最重要的字模文件。

注意，为了将代码下载到手机端调试，需要将手机与 Mac 用数据线相连，最重要的是需要有证书，否则无法下载到手机。证书需要一个 Apple 开发者账号才能获取。经过个人测试发现，我个人的国区账号无法成为开发者，但试了自己美区的账号就可以成为免费开发者，原因不明

测试平台：MacBook Pro 13-inch M1, 2020, Sonoma 14.5，手机为 iPhone 12 Pro Max, iOS 17.5.1

---

## 文件夹

### Smart Screen

核心代码都在此文件夹中

- Asset.xcassets：资源数据
- Bitmap.swift：负责读取点阵文件 gbk16.bin 二进制文件，并且进行下标偏置运算，得到对应的 32 字节点阵数据
- BLE.swift：负责与 MCU 建立 BLE 连接，通过向特征写入数据的方式进行文字传输和滚动参数控制
- ContentView.swift：负责页面布局和各类函数的调用
- Smart_ScreenApp.swift：主函数
- KeyboardObserving.swift：键盘观察器，用于收起键盘

### Assets.xcassets

存放了本 App 的图标

### Resource

存放了点阵文件 GBK16.bin

---

## 已知问题

在打开手机 App 的时候，MCU 可能会突然卡住，重启 MCU 几次即可解决
