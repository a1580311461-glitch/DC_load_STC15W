# DC_load_STC15W — 基于 RS-485 总线的直流电子负载

基于 **STC15W 系列 8051 单片机**的开源直流电子负载固件，支持 **Modbus RTU** 通信协议（遵循 GB/T 19582 国家标准），适用于电源测试、电池放电测试与工业自动化场景。

## ✨ 功能特性

- **恒流 / 恒压 / 恒功率闭环控制**：基于 ADC 实时采样 + 增强型 PWM 驱动，控制环路独立模块化
- **RS-485 总线通信**：标准 Modbus RTU 从站协议，支持多机挂载与集中监控
- **完善的保护机制**：NTC 温度监测、过温保护、风扇自动调速
- **参数掉电保存**：EEPROM 存储设定值与校准参数，上电自动恢复
- **本地人机交互**：OLED 显示屏 + 实体按键，可脱离上位机独立使用
- **告警提示**：蜂鸣器开机提示与故障报警

## 🧩 软件架构

代码按功能模块划分，职责清晰，便于二次开发：

| 模块 | 文件 | 说明 |
|---|---|---|
| 应用入口 | `Sources/main.c` | 系统初始化、10ms 按键扫描、100ms 控制/显示任务调度 |
| 电流控制 | `control.c` | 恒流/恒压控制环路、实际电流/电压计算 |
| 保护控制 | `control.c` | 过压/过流/过温保护 + 风扇调速 |
| 通信协议 | `modbus.c` | Modbus RTU 协议栈，基于 UART 轮询（非阻塞） |
| 采样 | `adc.c` | 电压/电流/NTC 温度 ADC 采样 |
| 驱动 | `pwm.c` / `fan.c` / `buzzer.c` / `oled.c` | 增强型 PWM、软件 PWM 风扇、蜂鸣器、OLED 驱动 |
| 人机交互 | `display.c` / `key.c` | UI 界面状态机、按键扫描 |
| 存储 | `eeprom.c` | 参数保存与上电恢复（含延迟保存机制） |
| 外设 | `uart.c` / `timer.c` / `clock.c` / `port.c` | 串口（UART1/UART3）、定时器、时钟、GPIO |

主循环采用**时间片轮询架构**（无阻塞延时）：

```
while(1) {
    Modbus_Poll();              // Modbus 事件处理
    Key_Task();                 // 每 10ms 按键扫描
    // 每 100ms：
    //   更新实时电压/电流/温度 → 保护检查+风扇调速 → 控制环路 → UI 刷新
}
```

## 🔧 硬件

- 主控：STC15W 系列单片机（具体型号见 `Sources/inc/config.h`）
- 通信：RS-485 收发器（UART1），UART3 预留调试用
- 显示：OLED（SPI/I2C，见 `oled.c`）
- 采样：片内 12 位 ADC + 外部分压/采样电阻网络

> 【TODO：请补充实际硬件参数：负载电压/电流范围、功率、MOS 管型号、采样电阻参数等，并可附原理图链接】

## 📡 通信协议

支持标准 **Modbus RTU** 协议：

- 规范文档：[`GB-T19582.1-2008_基于Modbus协议的工业自动化网络规范_第1部分_Modbus应用协议.md`](./GB-T19582.1-2008_基于Modbus协议的工业自动化网络规范_第1部分_Modbus应用协议.md)
- 串行链路实现：[`GB-T19582.2-2008_基于Modbus协议的工业自动化网络规范_第2部分_Modbus 协议在串行链路上的实现指南.md`](./GB-T19582.2-2008_基于Modbus协议的工业自动化网络规范_第2部分_Modbus 协议在串行链路上的实现指南.md)
- 附 MThings 上位机配置示例：[`mthings_cfg_demo.mthings`](./mthings_cfg_demo.mthings)

> 【TODO：请补充寄存器表（功能码、寄存器地址、数据格式、波特率/校验位）】

## 🛠️ 构建与烧录

### 方式一：EIDE + VSCode（推荐）

仓库已包含 `.eide` 与 `.vscode` 配置：

1. 安装 [VSCode](https://code.visualstudio.com/) + [EIDE 插件](https://github.com/github0null/eide)
2. 用 VSCode 打开 `DC_load_STC15W.code-workspace`
3. 安装 EIDE 提示的 8051 工具链（SDCC / Keil C51）
4. 点击 Build，生成的 hex 文件用 [stc-isp](https://www.stcaimcu.com/) 烧录

### 方式二：Keil µVision

直接打开 `DC_load_STC15W.uvproj` 编译即可。

## 📚 文档资料

| 文件 | 说明 |
|---|---|
| `MThings使用手册.md` | MThings 上位机调试工具使用说明 |
| `1 (2).pdf` | 【请补充说明】 |
| `基于485总线的直流电子负载-打分表V6.3(英文).pdf` | 项目设计评分表（英文版） |

## 🗺️ 路线图

- [ ] 补充寄存器表与通信协议文档
- [ ] 补充硬件原理图与 BOM
- [ ] 完善恒功率模式
- [ ] 支持更多 STC15 型号
- [ ] 编写上位机示例（Python / MThings）

## 🤝 贡献

欢迎 Issue 和 Pull Request！提交前请：

1. 遵循已有的 `.clang-format` 代码风格
2. 保持模块化的文件组织，新增功能请新建独立模块文件

## 📄 许可证

【TODO：建议添加 MIT License —— 在仓库页面点击 `Add file → Create new file`，文件名输入 `LICENSE`，选择 MIT License 模板即可】

## ⚠️ 免责声明

本项目为开源学习项目，使用者需自行承担因使用本固件或硬件设计造成的一切后果。大功率电子负载涉及发热与安全风险，请确保散热与电气设计满足实际负载要求。
