#ifndef __display_H
#define __display_H

#include "def.h"

// 系统状态枚举
typedef enum {
    SYS_STATE_START_PAGE = 0,           // 启动页
    SYS_STATE_MONITOR_PAGE,             // 功能页
    SYS_STATE_SETTING_PAGE,             // 设置页
    SYS_STATE_AUTO_TEST_PAGE,           // 自动测试页
    SYS_STATE_ALARM_OVERLAY,       // 报警覆盖页
} SystemState_e;

// 工作模式枚举
typedef enum {
    OP_MODE_CC = 0,
    OP_MODE_CR,
    OP_MODE_CP,
} OpMode_e;

// 设置页子状态
typedef enum {
    SETTING_SUB_LIST = 0,          // 浏览菜单列表
    SETTING_SUB_EDIT,              // 编辑数值
    SETTING_SUB_MODE_SELECT,       // 选择 CC/CR/CP 模式
    SETTING_SUB_FACTORY_CONFIRM    // 恢复出厂确认
} SettingSubState_e;

// 负载调整率测试子状态
typedef enum {
    AUTO_TEST_SUB_IDLE = 0,             // 待机/参数设置
    AUTO_TEST_SUB_MEASURE_NOLOAD,       // 空载测量中
    AUTO_TEST_SUB_MEASURE_LOAD,         // 带载测量中
    AUTO_TEST_SUB_RESULT,               // 显示结果
} AutoTestSubState_e;

typedef struct {
    uint16_t real_time_voltage_mV;         // 实时电压值，单位毫伏
    uint16_t real_time_current_mA;         // 实时电流值，单位毫安
    OpMode_e op_mode;                      // 当前工作模式
    uint16_t setting_current_mA;           // 设置电流值，单位毫安
    uint16_t setting_resistance_mOhm;       // 设置电阻值，单位毫欧
    uint16_t setting_power_mW;             // 设置功率值，单位毫瓦
    uint16_t real_time_temperature_C;      // 实时温度值，单位摄氏度
    uint16_t device_address;                 // 设备地址
    uint16_t max_current_mA;                   // 最大电流值，单位毫安
    uint16_t max_voltage_mV;                   // 最大电压值，单位毫伏
    uint16_t max_temperature_C;                // OTP 保护阈值，单位摄氏度
    uint8_t is_over_voltage;                     // 过压报警标志
    uint8_t is_over_temperature;                 // 过温报警标志
    uint16_t boot_count;                         // 启动次数
    uint8_t auto_test_running;                   // 负载调整率测试运行中（用于抑制风扇噪声）
} UIData_t;

typedef struct {
    SystemState_e current_page;         // 当前显示页面
    UIData_t ui_data;                  // 显示数据
    uint8_t is_changed;            // 页面切换标志
} DisplayPage_t;

extern DisplayPage_t xdata sys_ui;  // 系统UI状态结构体

void UI_Init(void);
void UI_Task(void);

#endif