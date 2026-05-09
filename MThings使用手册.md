## 摩尔信使 （MThings）使用手册

## 关于本手册

本手册主要针对长念（上海）技术开发有限公司自主研发的“摩尔信使（MThings）”软件。

## 目的

本手册目的在于向读者提供该软件的使用方法。

## 手册使用

在使用本产品前请仔细阅读本手册， 手册内容将不断更新、修正，但难免存在与使用稍有不符或错误的情况，请及时在微信公众号 “MThings 技术服务”上留言给我们。

如需更多使用介绍，请打开 B 站视频，搜索“MThings”，观看入门介绍视频。

## 观看入门视频

## 1 软件概述

摩尔信使（MThings）是一款基于 MODBUS 协议簇，面向工业控制现场运维人员的上位机软件。

## 1.1 关键功能

系统组态

实时告警

Linux发行版适配

历史数据

数据网关

报文解析

动态数据仿真

批量写

轮询

曲线录波

逻辑控制

自定义报文

MQTT转发

国产化硬件

报文记录

时延测试

设备丢包测试

通道丢包测试

串

RS485

TCP

UDP

以太网

灵活组网

适应非标

中按需定制

永久免费

## 1.2 版本差异

软件分为两个版本：个人版、商用版。

个人版面向个人用途，功能永久免费，不提供技术服务，不承担任何使用后果，同时用户所构建的系统禁止用于商业用途。

商用版面向工商业用户，提供全面的功能，需付费获得授权方可使用，授权均为一次性永久生效，提供售后技术服务。

<table><tr><td rowspan=1 colspan=1>功能项</td><td rowspan=1 colspan=1>个人版</td><td rowspan=1 colspan=1>商用版</td></tr><tr><td rowspan=1 colspan=1>系统组态</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>逻辑控制</td><td rowspan=1 colspan=1>了</td><td rowspan=1 colspan=1>→</td></tr><tr><td rowspan=1 colspan=1>历史数据</td><td rowspan=1 colspan=1>仅限一台主机设备</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>告警数据</td><td rowspan=1 colspan=1>最多3条告警配置</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>数据网关</td><td rowspan=1 colspan=1>×</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>MQTT转发</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>Linux</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>定制部署</td></tr><tr><td rowspan=1 colspan=1>通道管理</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>设备管理</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>用户管理</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>设备数据</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>模拟主机</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>模拟从机</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>实时数据曲线</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>链路统计</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>报文测试</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>辅助工具</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td></tr></table>

## 2 维护安装

## 2.1 下载

下载地址：http://gulink.cn/

## 2.2 安装

## 2.2.1 准备

个人电脑、工控机或触摸屏一体机。

显示器分辨率：推荐使用分辨率1920\*1080。

支持 Win7 及以上版本，兼容 32/64位。

## 2.2.2 安装

1. 下载软件包 MThings_Pack.exe；

2. 双击开始安装，根据步骤提示依次执行即可；

3. 双击图标即可启动软件。

## 3 功能详述

## 3.1 基本概念

## 3.1.1 设备

设备即通信节点，包含两类：模拟主机设备、模拟从机设备。

模拟主机设备：软件作为主机接入多个从机设备时，用户需要在软件中给每一个从机设备创建一个对应的模拟主机，不同的模拟主机通过设备地址区分。

<!-- image-->

模拟从机设备：创建的模拟从机设备一般用于 MODBUS从机仿真，可接入其他上位机软件或采集设备。

## 3.1.2 数据

MODBUS 标准中定义4 种数据块类型：

<table><tr><td rowspan=1 colspan=1>数据块</td><td rowspan=1 colspan=1>单位长度</td><td rowspan=1 colspan=1>访问类型</td><td rowspan=1 colspan=1>前导数</td><td rowspan=1 colspan=1>功能码</td></tr><tr><td rowspan=1 colspan=1>线圈状态</td><td rowspan=1 colspan=1>1 Bit</td><td rowspan=1 colspan=1>RW</td><td rowspan=1 colspan=1>0</td><td rowspan=1 colspan=1>0X01、0X05、0X0F</td></tr><tr><td rowspan=1 colspan=1>离散输入</td><td rowspan=1 colspan=1>1 Bit</td><td rowspan=1 colspan=1>RO</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1>0X02</td></tr><tr><td rowspan=1 colspan=1>保持寄存器</td><td rowspan=1 colspan=1>2 Bytes</td><td rowspan=1 colspan=1>RW</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>0X03、0X06、0X10</td></tr><tr><td rowspan=1 colspan=1>输入寄存器</td><td rowspan=1 colspan=1>2 Bytes</td><td rowspan=1 colspan=1>RO</td><td rowspan=1 colspan=1>3</td><td rowspan=1 colspan=1>0X04</td></tr></table>

基于数据块的单位长度，可将4种数据块分为两类：位（BIT）数据和寄存器(REG)数据，结合读写属性，数据块之间的关系如下图：

<!-- image-->

数据是数据块中的具体定义，不同数据块中的数据定义是相互独立的。一条数据定义可以包含多个连续 BIT/REG 数据地址。

一条数据的属性包括：隶属数据块、数据名称、数据地址、数据量、数据类型（协议类型）、数据系数（浮点数据被整形传输时涉及）、字节序（寄存器

数据量为 1、2、4 时涉及）、字序（寄存器数据量为 2、4 时涉及）。

## 3.1.3 传输类型和呈现类型

软件提供友好的数据可视化方法，支持以实际业务格式展示报文数据。

传输类型即 MODBUS 数据协议中定义的类型，通常以整形居多，但往往并非数据实际格式，使用者为了确认数据的正确性需要重新计算。不够直观的调试数据无疑严重影响调测效率和准确度。

软件支持用户指定每项数据的传输类型和呈现类型，调试过程中，软件自动按配置转换，界面呈现的数据将是数据的真实内容。

例如，从机上报电压数据10.5V，寄存器传输过程中数据则为0X0069，软件收到数据后，将整形数据乘以系数 0.1，界面呈现还原之后的数据 10.5V。

<!-- image-->

在录入数据配置时，软件提供的参数包括传输类型、呈现类型配置，他们的含义分别是：

<table><tr><td>参数</td><td>含义</td></tr><tr><td>传输类型</td><td>有符号整数、无符号整数、浮点数、字节流、位数据</td></tr><tr><td>呈现类型</td><td>浮点数、二进制数、十进制数、十六进制数、字节流、字符 串、纪元秒、枚举</td></tr></table>

软件支持多种传输类型和呈现类型的转换，对应关系如下：

<table><tr><td rowspan=2 colspan=1>传输类型呈现类型</td><td rowspan=1 colspan=1>有符号整数</td><td rowspan=1 colspan=1>无符号整数</td><td rowspan=1 colspan=1>浮点数</td><td rowspan=1 colspan=1>字节流</td><td rowspan=1 colspan=1>位数据</td></tr><tr><td rowspan=1 colspan=1>寄存器个数：1、2、4</td><td rowspan=1 colspan=1>寄存器个数：1、2、4</td><td rowspan=1 colspan=1>寄存器个数：2、4</td><td rowspan=1 colspan=1>寄存器个数：1~125</td><td rowspan=1 colspan=1>线圈状态、离散输入</td></tr><tr><td rowspan=1 colspan=1>浮点数</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>↓</td><td rowspan=1 colspan=1>了</td><td rowspan=1 colspan=1>x</td><td rowspan=1 colspan=1>X</td></tr><tr><td rowspan=1 colspan=1>二进制数</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>【</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>×</td><td rowspan=1 colspan=1>√</td></tr><tr><td rowspan=1 colspan=1>十进制数</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>×</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>X</td></tr><tr><td rowspan=1 colspan=1>十六进制数</td><td rowspan=1 colspan=1>了</td><td rowspan=1 colspan=1>」</td><td rowspan=1 colspan=1>×</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>X</td></tr><tr><td rowspan=1 colspan=1>字节流</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>×</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>X</td></tr><tr><td rowspan=1 colspan=1>字符串</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>X</td></tr><tr><td rowspan=1 colspan=1>纪元秒</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>X</td><td rowspan=1 colspan=1>×</td><td rowspan=1 colspan=1>X</td></tr><tr><td rowspan=1 colspan=1>枚举</td><td rowspan=1 colspan=1>J</td><td rowspan=1 colspan=1>√</td><td rowspan=1 colspan=1>×</td><td rowspan=1 colspan=1>×</td><td rowspan=1 colspan=1>√</td></tr></table>

## 3.1.4 字序/字节序

通信调试中，字节序是个常见概念，比如本地字节序、网络字节序。MODBUS 协议中同时存在字序概念。

字节序和字序都代表数据在报文中的存储机制，其分为两种：大端（Big-Endian）和小端（Little-Endian）。

MODBUS 数据帧中对关键字段（例如寄存器地址）都定义为大端字节序，但是数据部分在不同厂家的定义方式可能不同，甚至同一厂家某一型号设备中，对不同数据的字节序定义也不统一。

基于 MODBUS 报文的字节排序，大端字节序即数据高字节在前，低字节在后，小端字节序相反。例如：

01 03 04 12 34 56 78 81 07 （读保持寄存器响应报文）

其中，回复数据长度 4Bytes, 如原始数据即为 0X12345678，从报文看，其字节序即为大端，字序为大端。

但是，MODBUS 寄存器数据在传输 4字节浮点型数据时，有些设备厂家同时给出字序要求，并且明确声明字序为小端，即当原始数据是0X12345678时，传输它的报文字节流应该这样：

01 03 04 56 78 12 34 66 D5

软件为了支撑更好的适配不同厂家数据协议的定义方式，对字节序和字序做出如下处理：

每个数据都可以独立配置其字节序和字序。

寄存器数量为1、2、4 时，支持字节序可配置，涵盖 2、4、8 字节数据。

寄存器数据量为2、4 时，支持字序可配置，涵盖 4 字节、8 字节数据。

其他寄存器数据量的数据，字序、字节序默认为大端。

<table><tr><td rowspan=1 colspan=1>字节顺序</td><td rowspan=1 colspan=1>寄存器个数</td><td rowspan=1 colspan=1>字节序</td><td rowspan=1 colspan=1>字序</td></tr><tr><td rowspan=1 colspan=1>AB</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1>大端</td><td rowspan=1 colspan=1>二</td></tr><tr><td rowspan=1 colspan=1>BA</td><td rowspan=1 colspan=1>1</td><td rowspan=1 colspan=1>小端</td><td rowspan=1 colspan=1>二</td></tr><tr><td rowspan=1 colspan=1>AB CD</td><td rowspan=1 colspan=1>2</td><td rowspan=1 colspan=1>大端</td><td rowspan=1 colspan=1>大端</td></tr><tr><td rowspan=1 colspan=1>BA DC</td><td rowspan=1 colspan=1>2</td><td rowspan=1 colspan=1>小端</td><td rowspan=1 colspan=1>大端</td></tr><tr><td rowspan=1 colspan=1>CD AB</td><td rowspan=1 colspan=1>2</td><td rowspan=1 colspan=1>大端</td><td rowspan=1 colspan=1>小端</td></tr><tr><td rowspan=1 colspan=1>DC BA</td><td rowspan=1 colspan=1>2</td><td rowspan=1 colspan=1>小端</td><td rowspan=1 colspan=1>小端</td></tr><tr><td rowspan=1 colspan=1>AB CD EF GH</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>大端</td><td rowspan=1 colspan=1>大端</td></tr><tr><td rowspan=1 colspan=1>BA DC FE HG</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>小端</td><td rowspan=1 colspan=1>大端</td></tr><tr><td rowspan=1 colspan=1>GH EF CD AB</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>大端</td><td rowspan=1 colspan=1>小端</td></tr><tr><td rowspan=1 colspan=1>HG FE DC BA</td><td rowspan=1 colspan=1>4</td><td rowspan=1 colspan=1>小端</td><td rowspan=1 colspan=1>小端</td></tr></table>

## 3.2 使用流程

<!-- image-->

## 3.3 界面功能分区

<table><tr><td rowspan=1 colspan=9>填写名称园品丨8</td></tr><tr><td rowspan=1 colspan=1>序号</td><td rowspan=1 colspan=1>名称</td><td rowspan=1 colspan=1>数值</td><td rowspan=1 colspan=1>指令</td><td rowspan=1 colspan=1>单位</td><td rowspan=1 colspan=1>范围</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>地址</td><td rowspan=1 colspan=1>数量</td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>输出功率</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>kW</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>输入寄存器(RO)</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>运行状态</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>输入寄存器(RO)</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>运行时长</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>Hour</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>输入寄存器(RO)</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>今日电量</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1><img src="images/2c57a33fffa241ec062989f1f8fb1374d3a7aaee60ec817f294e0cc3ee799801.jpg"/></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>输入寄存器(RO)</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>累计电量</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>输入寄存器(RO)</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>AC电压</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>输入寄存器(RO)</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>AC电流</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>输入寄存器(RO)</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>DC电压</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>输入寄存器(RO)</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr><tr><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>DC电流</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1>输入寄存器(RO)</td><td rowspan=1 colspan=1></td><td rowspan=1 colspan=1></td></tr></table>

<!-- image-->

主菜单：用于切换“主功能页面”。

<!-- image-->

设备列表：显示设备信息和状态，双击后可以编辑设备属性信息。

❸ 更多功能：作为配置文件操作、通道管理、系统参数、用户管理、授权管理等功能入口。

<!-- image-->

❹ 报文监控：查阅和管理各链接的通信报文。

<!-- image-->

6 主功能页面：对应“主菜单”，提供主功能操作界面。

更多功能菜单内容如下图所示：

## 3.4 配置文件管理

配置文件包含完整的软件使用过程配置信息，主要包括：通道列表、通道参数、设备列表、设备高级参数、数据配置。

新建配置

凹打开配置

另存配置

回保存配置

白近期使用的配置

 新建配置：用于创建一个新的工程配置文件，首次使用可以不用新建配置，软件退出前将提示配置存储。

 打开配置：通过导入功能切换工程配置文件。

 另存配置：通过另存为操作导出复制当前工程配置，同时当前工程配置文件切换到新路径。

 保存配置：及时存盘工程配置文件。

 近期使用的配置：快速打开近期配置文件。

## 注意

当打开多个软件并指向同一份配置文件时，配置数据将会被重复覆盖，最终的配置文件内容与最近一次保存操作的内容一致。

## A警告

禁止将配置文件放在软件包路径内，避免软件升级时被误删除！

## 3.5 通道管理

软件支持以串口、TCP链接、UDP单播的方式与目标机建立通信，这三种通信方式统称为通道。

通道是设备的关键属性，通道确定设备通信方式，是创建设备的前提。

<!-- image-->  
通过更多功能菜单中的“通道管理”子项，即可进入该功能页面。

## 3.5.1 串口通道

<!-- image-->

通过“更多功能”菜单中的“通道管理”进入通道管理页面，查看当前 PC中实际存在的串口列表。

软件启动后，自动更新一次串口列表。如上图所示，串口相关操作如下：

<!-- image-->

选择通道类型，点击“串口通道”按钮。

❷ 当软件开启后，插入 USB转 RS485 模块，单击“刷新”，可将新串口添加至列表中，如未出现新串口，请检查模块驱动。当通道列表中出现无效串口（串口不存在且无挂靠设备）时，单击“刷新”，可将将这些串口从数据配置中移除。

❸ 如需修改串口参数，单击“配置”按钮进入“串口参数配置”页面❻。

❹ 查看通道状态，通道状态含义如下：已关闭、已连接、故障。

❺ 手动打开或关闭串口。添加设备前，无需手动开启。手动模式可用于批量控制通道上挂靠的所有设备，使其启动或停止。

❻ 串口参数配置页面。

## 串口通道状态定义如下：

<table><tr><td>通道状态</td><td>含义</td></tr><tr><td>已关闭</td><td>软件未启用该串口或被手动关闭</td></tr><tr><td>已连接</td><td>软件成功打开并初始化串口</td></tr><tr><td>故障</td><td>软件打开串口失败，原因可能是串口被其它软件占用或设 置的串口参数不合法</td></tr></table>

串口链接参数定义如下：

<table><tr><td>参数</td><td>含义</td></tr><tr><td>通道名称</td><td>操作系统分配信息，不可修改</td></tr><tr><td>传输模式</td><td>用于指定当前串口上运行的传输协议类型，可选择 "MODBUS-RTU"或"MODBUS-ASCII"</td></tr><tr><td>字符类型</td><td>当传输模式选择"MODBUS-ASCII”时，通过该参数指定协议 字符使用大写或小写，默认为标准协议要求的大写方式</td></tr><tr><td>波特率</td><td>支持1200、2400、4800、9600、19200、38400、57600、</td></tr><tr><td></td><td>115200bps，如设置为其它数值将导致串口打开失败</td></tr><tr><td></td><td>校验方式指定串口字节中数据校验方式 指定串口字节数据中数据位个数。如 MODBUS-ASCII协议中</td></tr><tr><td>数据位</td><td>要求数据位为7Bit时，请修改该参数</td></tr><tr><td>停止位</td><td>指定串口字节中停止位长度</td></tr><tr><td>流控类型</td><td>默认无流控，根据实际应用按需修改</td></tr></table>

## 注意

如使用虚拟串口，主从设备软件应使用配对的串口

## 3.5.2 网络通道

<!-- image-->  
通过“更多功能”菜单中的“通道管理”进入通道管理页面。

首次使用时，无网络通道，需用户根据需要创建。如上图所示，网络相关操作如下：

<!-- image-->

单击“新增网络链接”按钮，可创建一路新网络通道。

<!-- image-->

单击“移除网络链接”，删除通道列表中已被勾选的网络通道。

<!-- image-->

❸ 网络通道参数配置页面。

网络通道状态定义如下：

<table><tr><td>通道状态</td><td>含义</td></tr><tr><td>已关闭</td><td>软件未启用该链接</td></tr><tr><td></td><td></td></tr></table>

摩尔信使（MThings）使用手册
<table><tr><td></td><td>链接被对端断开 链接被手动关闭</td></tr><tr><td>已连接</td><td>网络通道已成功建链</td></tr><tr><td>连接中</td><td>TCP 客户端：正在连接服务端</td></tr><tr><td></td><td>TCP 服务器：正在等待接入客户端</td></tr><tr><td>故障</td><td>TCP 服务端：本地端口被占用</td></tr><tr><td></td><td>UDP 单播：本地端口被占用</td></tr><tr><td></td><td></td></tr></table>

## 网络通道参数定义如下：

<table><tr><td>参数</td><td>含义</td></tr><tr><td>通道名称</td><td>统一分配，不可修改</td></tr><tr><td>链接模式</td><td>选择网络层的链接角色，可选择TCP客户端、TCP 服务端、 UDP单播</td></tr><tr><td>重新建链周 期 (秒)</td><td>当链接模式选择为“TCP客户端&quot;，通过设置重新建链周期， 可指示软件按照指定时间间隔重新建链，通过这种方式可以模 拟短链接。默认周期为0，代表长链接。</td></tr><tr><td>链接空闲保 持时间 (秒)</td><td>当链接模式选择为“TCP客户端”或“TCP 服务端”时，链 接建立后，如果在参数指定时间内无任何数据交互，软件将主 动关闭链接。</td></tr><tr><td>传输模式</td><td>用于指定当前链接上运行的传输协议类型，可选择 &quot;MODBUS-RTU&quot;、 &quot;MODBUS-ASCII&quot;、&quot;MODBUS-TCP (同步)&quot;、&quot;MODBUS-TCP (异步)&quot;</td></tr><tr><td>字符类型</td><td>当传输模式选择“MODBUS-ASCII”时，通过该参数指定协 议字符使用大写或小写，默认为标准协议要求的大写方式。</td></tr><tr><td>本地IP</td><td>该参数用户指定本地发起IP，可不配置，软件可自动择优。</td></tr></table>

<table><tr><td>本地端口</td><td>定本地端口。 当链接模式选择为“TCP客户端&quot;，该参数为只读数据，用于 呈现已连接状态下的本地发起端口号。</td><td>当链接模式选择为“TCP 服务端”或&quot;UDP单播&quot;，需指定绑</td></tr><tr><td>目标域名/IP</td><td>当链接模式选择为“TCP客户端&quot;，该参数用于指定服务端的 域名或 IP 地址。 当链接模式选择为“UDP单播&quot;，该参数用于指定数据发送对 象的IP 地址。</td><td></td></tr><tr><td>目标端口</td><td>务端）绑定端口号。 象的接收端口号。</td><td>当链接模式选择为“TCP客户端&quot;，该参数用于指定对端（服 当链接模式选择为&quot;UDP单播&quot;，该参数用于指定数据发送对</td></tr><tr><td>客户端 IP</td><td>当前服务端的客户端IP，其它IP地址发起的连接请求将被过 滤丢弃。如不需过滤功能，该参数请设置为&quot;--”或清空。</td><td>当链接模式选择为“TCP服务端&quot;，该参数用于指定允许接入</td></tr><tr><td>源端口</td><td>当链接模式选择为“TCP 服务端&quot;，该参数为只读数据，用于 呈现已连接的客户端发起端口号。</td><td>为避免网络通道中报文交互过快，导致性能不必要的开销，软</td></tr><tr><td>最小帧间隔 (ms)</td><td>件默认设定最小帧间隔，如需提升或降低数据刷新率，可修改 该参数。</td><td></td></tr><tr><td>最大并发请 求数</td><td>当传输模式选择为&quot;MODBUS-TCP(异步)&quot;，该参数可控制 异步并发规模，调节并发请求个数</td><td></td></tr></table>