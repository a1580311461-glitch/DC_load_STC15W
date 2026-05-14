//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: pwm.c
// 文件描述: 
// 文件版本: V1.0
// 修改记录:
//   1. (2026-04-16) 创建文件
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#include "config.h"


//<<AICUBE_USER_INCLUDE_BEGIN>>
// 在此添加用户头文件包含  
//<<AICUBE_USER_INCLUDE_END>>


//<<AICUBE_USER_GLOBAL_DEFINE_BEGIN>>
// 在此添加用户全局变量定义、用户宏定义以及函数声明

//【注意：T2和T1不能重合，T1和T2不能大于周期，否则：周期翻倍，50%占空比】
#define PWM_CYCLE 1000  

//<<AICUBE_USER_GLOBAL_DEFINE_END>>



////////////////////////////////////////
// 增强型PWM初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void PWM_Init(void)
{
    PWM_CLK_SYSCLK();                   //设置系统时钟为PWM时钟源
    PWM_SetClockDivider(0);             //设置系统时钟的分频值 [SYSCLK/(n+1)]
    PWM_SetCounter(1000);               //使能PWM周期值

    PWM_C2_SwitchP37();                 //设置PWM2端口: PWM2 (P3.7)
    PWM_InitialC2High();                //设置PWM2通道初始电平为高电平
    PWM_SetC2T1Point(0);                //使能PWM2翻转点1的计数值
    PWM_SetC2T2Point(500);              //使能PWM2翻转点2的计数值
    PWM_EnableC2Output();               //使能PWM2通道输出

    PWM_Enable();                       //使能PWM功能

    //<<AICUBE_USER_PWM_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  

    PWM2_Setting();

    //<<AICUBE_USER_PWM_INITIAL_END>>
}



//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 在此添加用户函数实现代码  

// PWM2脉宽设置
void PWM2_SetWidth(unsigned int Width)
{
    if(Width >= PWM_CYCLE)
    {
        PWM_SetC2T2Point(PWM_CYCLE);    //差1个脉宽
        PWM_DisableC2Output();          //关闭PWM输出，使能GPIO
        PWM_IN = 1;                     //直接输出高电平
    }
    else if(Width < 1)
    {
        PWM_SetC2T2Point(1);            //差1个脉宽
        PWM_DisableC2Output();          //关闭PWM输出，使能GPIO
        PWM_IN = 0;                     //直接输出低电平
    }
    else
    {
        PWM_SetC2T2Point(Width);        //设置T2
        PWM_EnableC2Output();           //使能PWM输出
    }
}

void PWM2_Setting(void)
{
    //PWM2(P3.7)通道设置：
    PWM_Disable();    //【注意】配置前必须先Disable，配置完毕后再Enable！！《见手册P.1032，关于ENPMW控制位的重要说明》
    //所有ENPMW必须在其他所有的PWM设置（包括T1/T2翻转点的设置、初始电平的设置、PWM异常检测的设置以及PWM中断设置）都完成后，最后才能使能ENPMW位
    //PWM_InitialC2High();
    //设置PWM2通道初始电平为高电平
    PWM_InitialC2Low();
    //设置PWM2通道初始电平为低电平【注意：Aicube截止V1.01N版有Bug，设置初始电平为低电平没有效果】
    PWM_SetC2T1Point(0);
    //使能PWM2翻转点1的计数值
    PWM_SetC2T2Point(10);
    //使能PWM2翻转点2的计数值 1~1000 【注意：T2和T1不能重合，T1和T2不能大于周期，否则：周期翻倍，50%占空比】
    PWM_EnableC2Output();
    //使能PWM2通道输出，PWM端口受PWM波形发生器控制

    //PWM_DisableC2Output();
    //禁止PWM2通道输出，PWM端口为GPIO口，受端口寄存器的值控制
    PWM_IN = 0;
    //当禁止PWM输出时，PWM端口为GPIO口，需要设置端口寄存器的值为0[上电默认为1]

    PWM_Enable();
    //使能PWM功能
}


//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


