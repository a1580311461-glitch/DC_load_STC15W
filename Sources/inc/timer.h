//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: timer.h
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#ifndef __TIMER_H__
#define __TIMER_H__


//<<AICUBE_USER_DEFINE_BEGIN>>
// 在此添加用户宏定义  
//<<AICUBE_USER_DEFINE_END>>



void TIMER0_Init(void);
void TIMER1_Init(void);



//<<AICUBE_USER_EXTERNAL_DECLARE_BEGIN>>
// 在此添加用户外部函数和外部变量声明  

uint16_t Get_SysTick(void); // 获取系统滴答计数器的安全接口

extern uint16_t sys_tick;    // 系统滴答计数器

//<<AICUBE_USER_EXTERNAL_DECLARE_END>>


#endif
