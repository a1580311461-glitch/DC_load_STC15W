//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息
// 文件名称: config.h
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#ifndef __CONFIG_H__
#define __CONFIG_H__


//<<AICUBE_USER_DEFINE_BEGIN>>
// 在此添加用户宏定义

/* 指示灯控制宏 */
#define LED_ERR_ON()       do { P26 = 0; } while(0) // 错误指示灯亮
#define LED_ERR_OFF()      do { P26 = 1; } while(0) // 错误指示灯灭
#define LED_ERR_TOGGLE()   do { P26 = !P26; } while(0) // 错误指示灯翻转
#define LED_OUT_ON()       do { P27 = 0; } while(0) // 输出指示灯亮
#define LED_OUT_OFF()      do { P27 = 1; } while(0) // 输出指示灯灭
#define LED_OUT_TOGGLE()   do { P27 = !P27; } while(0) // 输出指示灯翻转
#define LED_485_ON()       do { P04 = 0; } while(0) // 485 指示灯亮
#define LED_485_OFF()      do { P04 = 1; } while(0) // 485 指示灯灭
#define LED_485_TOGGLE()   do { P04 = !P04; } while(0) // 485 指示灯翻转


/* OUTPUT_EN 控制宏 — 用软件变量跟踪状态，开漏模式下读引脚不可靠 */
extern unsigned char output_en_state;
#define OUTPUT_EN_ON()      do { OUTPUT_EN = 1; output_en_state = 1; } while(0)
#define OUTPUT_EN_OFF()     do { OUTPUT_EN = 0; output_en_state = 0; } while(0)
#define OUTPUT_EN_TOGGLE()  do { output_en_state = !output_en_state; OUTPUT_EN = output_en_state; } while(0)
#define OUTPUT_EN_IS_ON     (output_en_state)


/*RS485 方向控制宏*/
#define RS485_SET_TX()      do {P02 = 1;} while(0)   // P02 作为 RS485 方向控制引脚，1=发送，0=接收
#define RS485_SET_RX()      do {P02 = 0;} while(0)


//<<AICUBE_USER_DEFINE_END>>


#define __ENCODING              "UTF-8" //DO NOT DELETE or MODIFY


#define FOSC                    11059200UL
#define SYSCLK                  FOSC
#define MAIN_Fosc               FOSC

#include <stc15.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <intrins.h>

#include "def.h"
#include "stc15w_def.h"


//<<AICUBE_USER_INCLUDE_BEGIN>>
// 在此添加用户头文件包含

#include "key.h"
#include "oled.h"
#include "oledfont.h"
#include "display.h"
#include "control.h"
#include "ntc.h"
#include "eeprom.h"
#include "modbus.h"
#include "buzzer.h"
#include "fan.h"

//<<AICUBE_USER_INCLUDE_END>>


#define ADC_U                   P10
#define ADC_I                   P11
#define ADC_T                   P12
#define KEY_MODE                P13
#define KEY_INC                 P14
#define KEY_DEC                 P15
#define KEY_ON_OFF              P55
#define RxD                     P30
#define TxD                     P31
#define OUTPUT_EN               P33
#define BUZZER                  P35
#define PWM_IN                  P37
#define OLED_SDIN               P20
#define OLED_SCL                P21
#define FAN                     P22
#define LED_ERR                 P26
#define LED_OUT                 P27
#define RxD3                    P00
#define TxD3                    P01
#define LED_485                 P04


#include "port.h"
#include "clock.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"
#include "pwm.h"

void SYS_Init(void);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);



//<<AICUBE_USER_EXTERNAL_DECLARE_BEGIN>>
// 在此添加用户外部函数和外部变量声明
//<<AICUBE_USER_EXTERNAL_DECLARE_END>>


#endif
