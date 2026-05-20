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
static uint16_t xdata s_last_vcc = 0;   // 上次有效VCC，用于异常值回退
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
    uint16_t timeout;                   //超时计数器

    ADC_ActiveChannel(ch);              //选择ADC通道
    ADC_Start();                        //开始ADC转换
    timeout = 0;
    while (!ADC_CheckFlag()) {
        if (++timeout > 30000) {        //超时约20ms（远超正常转换时间~160us）
            ADC_ClearFlag();
            return 0;                   //超时返回0，避免卡死
        }
    }
    ADC_ClearFlag();                    //清除ADC转换完成中断标志
    res = ADC_ReadResult();             //读取ADC转换结果

    return res;                         //返回ADC结果
}



//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 在此添加用户函数实现代码

uint16_t ADC_Get_Vcc(void){
    uint32_t sum_gap = 0;
    uint16_t vcc;
    uint8_t i;

    P1ASF = 0;
    delay_us(10);

    // 平均读取8次带隙基准
    for (i = 0; i < 8; i++) {
        sum_gap += ADC_Convert(0);
    }

    if (sum_gap == 0) goto fail;

    SetP1nAnalogInput(0x00000007);
    delay_us(10);

    vcc = (uint32_t)s_Vref_BandGap_mV * 1023 * 8 / (uint16_t)sum_gap;

    // 合理性校验：系统供电约5V，VCC应在4000~5500mV范围
    // MOSFET开关瞬态会污染带隙读数导致VCC暴跌到3800甚至更低，此时复用上次有效值
    if (vcc >= 4000 && vcc <= 5500) {
        s_last_vcc = vcc;
        return vcc;
    }

fail:
    return s_last_vcc;  // 返回上次有效值（首次为0）
}

uint16_t ADC_Convert_mV(uint8_t ch)
{
    uint16_t vcc = ADC_Get_Vcc();       // 复用已验证的VCC（含异常值抛弃），同时完成P1ASF配置
    uint32_t sum = 0;
    uint8_t i;

    if (vcc == 0) return 0;

    // 此时P1ASF已由ADC_Get_Vcc设为0x07(P1.0~P1.2模拟使能)，直接读信号通道
    // 信号通道过采样：连续读 16 次累加
    for (i = 0; i < 16; i++) {
        sum += ADC_Convert(ch);
    }

    // result = avg_signal / 1023 × VCC
    return (uint32_t)(sum >> 4) * vcc / 1023;
}


//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


