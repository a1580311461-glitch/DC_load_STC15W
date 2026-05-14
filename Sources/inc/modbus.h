#ifndef __MODBUS_H__
#define __MODBUS_H__


// MODBUS功能码宏
#define MODBUS_FC_READ_HOLDING    0x03
#define MODBUS_FC_READ_INPUT      0x04
#define MODBUS_FC_WRITE_SINGLE    0x06
#define MODBUS_FC_WRITE_MULTIPLE  0x10


// MODBUS寄存器地址宏
// 读写区
#define MODBUS_REG_DEVICE_ADDR        0x0000    // 设备地址（R/W, 0-247）
#define MODBUS_REG_WORK_MODE          0x0001    // 工作模式（R/W, 0=CC, 1=CR, 2=CP）
#define MODBUS_REG_SETTING_CURRENT    0x0002    // 设置电流值（R/W, mA）
#define MODBUS_REG_SETTING_RESISTANCE 0x0003    // 设置电阻值（R/W, mΩ）
#define MODBUS_REG_SETTING_POWER      0x0004    // 设置功率值（R/W, mW）
#define MODBUS_REG_MAX_CURRENT        0x0005    // 最大电流值（R/W, mA）
#define MODBUS_REG_MAX_VOLTAGE        0x0006    // 最大电压值（R/W, mV）
#define MODBUS_REG_MAX_TEMPERATURE    0x0007    // 最大温度值（R/W, ℃）

// 只读区
#define MODBUS_REG_REAL_TIME_VOLTAGE      0x1000   // 实时电压值（R, mV）
#define MODBUS_REG_REAL_TIME_CURRENT      0x1001   // 实时电流值（R, mA）
#define MODBUS_REG_REAL_TIME_TEMPERATURE  0x1002   // 实时温度值（R, ℃×100）
#define MODBUS_REG_OUTPUT_STATE           0x1003   // 输出状态（R, 0=OFF, 1=ON）
#define MODBUS_REG_OVERVOLTAGE_FLAG       0x1004   // 过压标志（R, 0=未触发, 1=触发）
#define MODBUS_REG_OVERTEMP_FLAG          0x1005   // 过热标志（R, 0=未触发, 1=触发）
#define MODBUS_REG_BOOT_COUNT             0x1006   // 启动次数（R, 0-65535）


// MODBUS帧缓冲区大小
#define MODBUS_FRAME_SIZE_MAX  64


// MODBUS函数声明
void Modbus_Init(void);       // MODBUS初始化
void Modbus_Poll(void);       // 主循环中调用，处理接收帧


// MODBUS异常码宏
#define MODBUS_EX_ILLEGAL_FUNC     0x01   // 不支持的功能码
#define MODBUS_EX_ILLEGAL_ADDR     0x02   // 非法的数据地址
#define MODBUS_EX_ILLEGAL_VALUE    0x03   // 非法的数据值
#define MODBUS_EX_SLAVE_FAILURE    0x04   // 从机设备故障


#endif