//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: timer.c
// 文件描述: 
// 文件版本: V1.0
// 修改记录:
//   1. (2026-04-16) 创建文件
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#include "config.h"


//<<AICUBE_USER_INCLUDE_BEGIN>>
// 在此添加用户头文件包含

#include "fan.h"

//<<AICUBE_USER_INCLUDE_END>>


//<<AICUBE_USER_GLOBAL_DEFINE_BEGIN>>
// 在此添加用户全局变量定义、用户宏定义以及函数声明 

volatile uint16_t sys_tick = 0;    // 系统滴答计数器

//<<AICUBE_USER_GLOBAL_DEFINE_END>>



////////////////////////////////////////
// 定时器0初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void TIMER0_Init(void)
{
#define T0_RELOAD               (65536 - (float)SYSCLK * 1 / 1000) //定时周期1毫秒

    TIMER0_TimerMode();                 //设置定时器0为定时模式
    TIMER0_1TMode();                    //设置定时器0为1T模式
    TIMER0_Mode0();                     //设置定时器0为模式0 (16位自动重载模式)
    TIMER0_DisableGateINT0();           //禁止定时器0门控
    TIMER0_SetIntPriority(0);           //设置中断为低优先级
    TIMER0_EnableInt();                 //使能定时器0中断
    TIMER0_SetReload16(T0_RELOAD);      //设置定时器0的16位重载值
    TIMER0_Run();                       //定时器0开始运行

    //<<AICUBE_USER_TIMER0_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_TIMER0_INITIAL_END>>
}

////////////////////////////////////////
// 定时器1初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void TIMER1_Init(void)
{
#define T1_RELOAD               (65536 - (float)SYSCLK * 250 / 1000000) //定时周期250微秒

    TIMER1_TimerMode();                 //设置定时器1为定时模式
    TIMER1_1TMode();                    //设置定时器1为1T模式
    TIMER1_Mode0();                     //设置定时器1为模式0 (16位自动重载模式)
    TIMER1_DisableGateINT1();           //禁止定时器1门控
    TIMER1_SetIntPriority(0);           //设置中断为低优先级
    TIMER1_EnableInt();                 //使能定时器1中断
    TIMER1_SetReload16(T1_RELOAD);      //设置定时器1的16位重载值
    TIMER1_Run();                       //定时器1开始运行

    //<<AICUBE_USER_TIMER1_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_TIMER1_INITIAL_END>>
}

////////////////////////////////////////
// 定时器4初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void TIMER4_Init(void)
{
#define T4_RELOAD               (65536 - (float)SYSCLK * 1 / 1000) //定时周期1毫秒

    TIMER4_TimerMode();                 //设置定时器4为定时模式
    TIMER4_1TMode();                    //设置定时器4为1T模式
    TIMER4_EnableInt();                 //使能定时器4中断
    TIMER4_SetReload16(T4_RELOAD);      //设置定时器4的16位重载值
    TIMER4_Run();                       //定时器4开始运行

    //<<AICUBE_USER_TIMER4_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_TIMER4_INITIAL_END>>
}


////////////////////////////////////////
// 定时器0中断服务程序
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void TIMER0_ISR(void) interrupt TMR0_VECTOR
{
    //<<AICUBE_USER_TIMER0_ISR_CODE1_BEGIN>>
    // 在此添加中断函数用户代码

    sys_tick++; // 1ms 加一次

    //<<AICUBE_USER_TIMER0_ISR_CODE1_END>>
}

////////////////////////////////////////
// 定时器1中断服务程序
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void TIMER1_ISR(void) interrupt TMR1_VECTOR
{
    //<<AICUBE_USER_TIMER1_ISR_CODE1_BEGIN>>
    // 在此添加中断函数用户代码  

    if (buzzer_active)
    {
        BUZZER = !BUZZER;   // 产生 2kHz 方波 (250us × 2 = 500us 周期)
    }
    else
    {
        BUZZER = 1;         // 不发声时引脚置高（NPN截止，蜂鸣器静音）
    }

    //<<AICUBE_USER_TIMER1_ISR_CODE1_END>>
}

////////////////////////////////////////
// 定时器4中断服务程序
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void TIMER4_ISR(void) interrupt TMR4_VECTOR
{
    //<<AICUBE_USER_TIMER4_ISR_CODE1_BEGIN>>
    // 在此添加中断函数用户代码

    // 软件PWM直接在中断内处理，消除跨文件调用的巨额压栈开销
    if (fan_duty == 0) {
        FAN = 0;                        // 停止
    } else if (fan_duty >= FAN_PWM_PERIOD) {
        FAN = 1;                        // 全速
    } else {
        if (++fan_cnt >= FAN_PWM_PERIOD) {
            fan_cnt = 0;
        }
        FAN = (fan_cnt < fan_duty); // 占空比控制
    }

    //<<AICUBE_USER_TIMER4_ISR_CODE1_END>>
}


//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 在此添加用户函数实现代码 

// 提供一个安全的获取时间接口
uint16_t Get_SysTick(void)
{
    uint16_t time;
    DisableGlobalInt();         // 关闭全局中断，进入临界区
    time = sys_tick;            // 安全读取 16 位变量
    EnableGlobalInt();          // 恢复全局中断
    return time;
}

//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


