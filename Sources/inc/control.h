#ifndef __CONTROL_H
#define __CONTROL_H

#define R_SENSEx100 10 // 电流采样电阻值，单位Ω，实际值0.1Ω，但为了避免浮点运算，放大了100倍，计算时注意缩放
#define AMP_GAIN 11  // 硬件放大倍数

// 宏定义：硬件相关参数
#define MAX_PWM_DUTY       1000  // 对应 pwm.c 中的 PWM_CYCLE

// 宏定义：步进控制参数
#define PWM_STEP_SIZE      5        // 每次步进改变的占空比数值 (值越大，逼近越快)
#define PWM_STEP_DELAY_MS  2        // 每次步进之间的延时时间 (毫秒，给硬件RC滤波和运放响应留出时间)

void Current_Control_Init(void);
uint16_t Current_Control_GetActualCurrent_mA(void);
void Current_Control_SetTarget(uint16_t target_mA);
void Current_Control_Tick(void);
void Current_Control_ResetDuty(void);

uint16_t Voltage_Control_GetActualVoltage_mV(void);

uint16_t Temperature_Control_GetActualTemperaturex100(void);

void Control_Tick(void);

void Control_Protect(void);

#endif 
