//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 锟节达拷锟斤拷锟斤拷锟矫伙拷锟侥硷拷头说锟斤拷锟斤拷息  
// 锟侥硷拷锟斤拷锟斤拷: config.h
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#ifndef __CONFIG_H__
#define __CONFIG_H__


//<<AICUBE_USER_DEFINE_BEGIN>>
// 锟节达拷锟斤拷锟斤拷锟矫伙拷锟疥定锟斤拷  

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
// 锟节达拷锟斤拷锟斤拷锟矫伙拷头锟侥硷拷锟斤拷锟斤拷

#include "key.h"
#include "oled.h"
#include "oledfont.h"
#include "display.h"
#include "control.h"
#include "ntc.h"
#include "settings.h"
#include "modbus.h"
#include "buzzer.h"

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
#define PWM_FAN                 P22
#define LED_ERR                 P26
#define LED_OUT                 P27
#define RxD3                    P00
#define TxD3                    P01
#define LED_485                 P04


#include "port.h"
#include "clock.h"
#include "iap.h"
#include "timer.h"
#include "uart.h"
#include "adc.h"
#include "pwm.h"

void SYS_Init(void);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);



//<<AICUBE_USER_EXTERNAL_DECLARE_BEGIN>>
// 锟节达拷锟斤拷锟斤拷锟矫伙拷锟解部锟斤拷锟斤拷锟斤拷锟解部锟斤拷锟斤拷锟斤拷锟斤拷  
//<<AICUBE_USER_EXTERNAL_DECLARE_END>>


#endif
