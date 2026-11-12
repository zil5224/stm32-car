# STM32多模式智能小车

基于 STM32F103C8 的小车固件，包含红外循迹、红外避障和红外遥控三种模式。程序使用 STM32F10x 标准外设库，通过 TIM4 的四路 PWM 控制电机，数码管显示当前模式。

## 功能与操作

上电默认进入模式 1。按一下模式按键，依次切换至模式 2、模式 3，再回到模式 1。

| 数码管 | 模式 | 行为 |
| --- | --- | --- |
| 1 | 红外循迹 | 读取左右循迹传感器，执行前进、左转、右转或停止。 |
| 2 | 红外避障 | 单侧检测到障碍时向另一侧避让，两侧同时检测到障碍时先停止，再原地右转。 |
| 3 | 红外遥控 | 轮询接收 NEC 风格红外帧，通过反码校验后执行方向或停止命令。 |

遥控模式支持前进、后退、原地左转、原地右转和停止。命令映射见 [`User/main.c`](User/main.c)。未收到新的有效帧时，小车保持上一次运动状态。调试时可通过 USART1 查看接收的红外码和命令字节。

## 硬件引脚

以下引脚来自固件的 GPIO 初始化，接线时请结合扩展板丝印。

| 功能 | STM32 引脚 |
| --- | --- |
| 模式按键 | PA15 |
| 左、右循迹传感器 | PB13、PB12 |
| 左、右避障传感器 | PA11、PA12 |
| 红外遥控接收 | PA8 |
| 电机 PWM（TIM4 CH1–CH4） | PB6、PB7、PB8、PB9 |
| 数码管 | PA0–PA7 |
| USART1 调试串口（9600，8N1） | PA9（TX）、PA10（RX） |

## 编译与下载

1. 安装 Keil MDK-ARM、ARM Compiler 5 和 `Keil::STM32F1xx_DFP`。原工程配置的版本是 ARM Compiler 5.06 update 5 和 STM32F1xx_DFP 2.2.0。
2. 用 µVision 打开 [`Project.uvprojx`](Project.uvprojx)，选择 `Target 1`，执行 **Project → Rebuild all target files**。
3. 构建成功后，固件位于 `Objects/Project.hex`。使用 ST-LINK 时，在 **Options for Target → Debug** 选择对应驱动并确认 Flash 下载设置，再执行 **Flash → Download**。

## 目录结构

| 路径 | 内容 |
| --- | --- |
| [`Project.uvprojx`](Project.uvprojx) | Keil 工程入口，目标芯片为 STM32F103C8 |
| [`User/`](User/) | 主循环、模式控制和中断入口 |
| [`Hardware/`](Hardware/) | 传感器、红外解码、电机 PWM、数码管和串口驱动 |
| [`System/`](System/) | 延时函数 |
| [`Start/`](Start/) | CMSIS、启动文件和系统时钟配置 |
| [`Library/`](Library/) | STM32F10x 标准外设库 |
| [`DebugConfig/`](DebugConfig/) | STM32F103C8 调试配置 |

## 代码来源

本项目在购买小车配件时随附的示例程序基础上进行了改进。STM32F10x 标准外设库和 CMSIS 源文件保留了原有版权声明。
