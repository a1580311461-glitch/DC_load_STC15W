//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: port.c
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
//<<AICUBE_USER_GLOBAL_DEFINE_END>>



////////////////////////////////////////
// P0口初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void PORT0_Init(void)
{
    SetP0nInitLevelHigh(PIN_ALL);       //设置P0初始化电平
    SetP0nQuasiMode(PIN_7 | PIN_6 | PIN_5 | PIN_4 | PIN_2 | PIN_1 | PIN_0); //设置P0.7,P0.6,P0.5,P0.4,P0.2,P0.1,P0.0为准双向口模式
    SetP0nHighZInputMode(PIN_3);        //设置P0.3为高阻输入模式

    //<<AICUBE_USER_PORT0_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_PORT0_INITIAL_END>>
}

////////////////////////////////////////
// P1口初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void PORT1_Init(void)
{
    SetP1nInitLevelHigh(PIN_ALL);       //设置P1初始化电平
    SetP1nQuasiMode(PIN_7 | PIN_6 | PIN_5 | PIN_4 | PIN_3); //设置P1.7,P1.6,P1.5,P1.4,P1.3为准双向口模式
    SetP1nHighZInputMode(PIN_2 | PIN_1 | PIN_0); //设置P1.2,P1.1,P1.0为高阻输入模式

    //<<AICUBE_USER_PORT1_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_PORT1_INITIAL_END>>
}

////////////////////////////////////////
// P2口初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void PORT2_Init(void)
{
    SetP2nInitLevelHigh(PIN_ALL);       //设置P2初始化电平
    SetP2nQuasiMode(PIN_7 | PIN_6 | PIN_5 | PIN_4 | PIN_3 | PIN_0); //设置P2.7,P2.6,P2.5,P2.4,P2.3,P2.0为准双向口模式
    SetP2nPushPullMode(PIN_2 | PIN_1);  //设置P2.2,P2.1为推挽输出模式

    //<<AICUBE_USER_PORT2_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_PORT2_INITIAL_END>>
}

////////////////////////////////////////
// P3口初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void PORT3_Init(void)
{
    SetP3nInitLevelHigh(PIN_ALL);       //设置P3初始化电平
    SetP3nQuasiMode(PIN_6 | PIN_4 | PIN_2 | PIN_1 | PIN_0); //设置P3.6,P3.4,P3.2,P3.1,P3.0为准双向口模式
    SetP3nPushPullMode(PIN_7 | PIN_5);  //设置P3.7,P3.5为推挽输出模式
    SetP3nOpenDrainMode(PIN_3);         //设置P3.3为开漏模式

    //<<AICUBE_USER_PORT3_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_PORT3_INITIAL_END>>
}



//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 在此添加用户函数实现代码  
//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


