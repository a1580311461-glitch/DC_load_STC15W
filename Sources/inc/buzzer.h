////////////////////////////////////////
// 文件名称: buzzer.h
// 文件描述: 无源蜂鸣器驱动模块头文件
// 文件版本: V1.0
// 修改记录:
//   1. (2026-05-09) 创建文件
////////////////////////////////////////

#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "def.h"

void Buzzer_Init(void);
void Buzzer_Tick(void);
void Buzzer_KeyBeep(void);
void Buzzer_PowerOn(void);
void Buzzer_SetAlarm(uint8_t enable);
void Buzzer_ShortTick(void);                         // 短促嘀嗒声(5ms)，连按数值变化时使用

extern volatile uint8_t buzzer_active; // Timer1 ISR 据此翻转 BUZZER 引脚

#endif
