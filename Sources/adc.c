//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: adc.c
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
static const uint16_t idata s_Vref_BandGap_mV _at_ 0xEF;
//<<AICUBE_USER_GLOBAL_DEFINE_END>>



////////////////////////////////////////
// ADC初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void ADC_Init(void)
{
    ADC_SetSpeedTo90Clocks();           //设置ADC速度
    ADC_ResultRightAlign();             //设置ADC结果右对齐(高字节存放高2位结果,低字节存放低8位结果)


    SetP1nAnalogInput(0x00000007);      //设置ADC通道为模拟端口
    ADC_ActiveChannel(0);               //选择ADC通道
    ADC_Enable();                       //使能ADC功能

    //<<AICUBE_USER_ADC_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_ADC_INITIAL_END>>
}

////////////////////////////////////////
// 获取ADC转换结果函数
// 入口参数: ch (ADC通道选择)
// 函数返回: ADC转换结果
////////////////////////////////////////
uint16_t ADC_Convert(uint8_t ch)
{
    uint16_t res;                       //定义保存ADC结果的变量

    ADC_ActiveChannel(ch);              //选择ADC通道
    ADC_Start();                        //开始ADC转换
    while (!ADC_CheckFlag());           //等待ADC转换完成
    ADC_ClearFlag();                    //清除ADC转换完成中断标志
    res = ADC_ReadResult();             //读取ADC转换结果

    return res;                         //返回ADC结果
}



//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 在此添加用户函数实现代码  

uint16_t ADC_Get_Vcc(void){
    uint16_t res;                       //定义保存ADC结果的变量
    uint16_t ADC_gap;                   //定义保存ADC基准电压转换结果的变量

    P1ASF = 0;
    delay_us(10);
    ADC_gap = ADC_Convert(0);
    ADC_gap = ADC_Convert(0);
    ADC_gap = ADC_Convert(0);

    if (ADC_gap == 0) {
    return 0;  // 或返回上次有效值
    }

    SetP1nAnalogInput(0x00000007);      //设置ADC通道为模拟端口
    delay_us(10);

    res = (uint32_t)s_Vref_BandGap_mV * 1023 / ADC_gap;  //计算Vcc电压的mV值

    return res;                         
}

uint16_t ADC_Convert_mV(uint8_t ch)
{
    uint16_t ADC_gap;                   //定义保存ADC基准电压转换结果的变量
    uint32_t sum = 0;
    uint8_t i;

    P1ASF = 0;
    delay_us(10);
    ADC_gap = ADC_Convert(0);
    ADC_gap = ADC_Convert(0);
    ADC_gap = ADC_Convert(0);

    SetP1nAnalogInput(0x00000007);      //设置ADC通道为模拟端口
    delay_us(10);

    // 信号通道过采样：连续读 8 次累加
    for (i = 0; i < 8; i++) {
        sum += ADC_Convert(ch);
    }

    return (uint32_t)s_Vref_BandGap_mV * (uint16_t)(sum >> 3) / ADC_gap;                         //返回ADC结果
}


//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


