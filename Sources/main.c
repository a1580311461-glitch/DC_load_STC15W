//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 锟节达拷锟斤拷锟斤拷锟矫伙拷锟侥硷拷头说锟斤拷锟斤拷息  
// 锟侥硷拷锟斤拷锟斤拷: main.c
// 锟侥硷拷锟斤拷锟斤拷: 
// 锟侥硷拷锟芥本: V1.0
// 锟睫改硷拷录:
//   1. (2026-04-16) 锟斤拷锟斤拷锟侥硷拷
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#include "config.h"                     //默认已包含stdio.h、intrins.h等头文件


//<<AICUBE_USER_INCLUDE_BEGIN>>
 
//<<AICUBE_USER_INCLUDE_END>>


//<<AICUBE_USER_GLOBAL_DEFINE_BEGIN>>
// 锟节达拷锟斤拷锟斤拷锟矫伙拷全锟街憋拷锟斤拷锟斤拷锟藉、锟矫伙拷锟疥定锟斤拷锟皆硷拷锟斤拷锟斤拷锟斤拷锟斤拷  

static uint16_t s_last_time_key = 0;  // 锟较达拷扫锟借按锟斤拷锟斤拷系统时锟戒，锟斤拷位为ms
static uint16_t s_last_time_100ms = 0; // 锟较达拷执锟斤拷100ms锟斤拷锟斤拷锟较低呈憋拷洌?锟斤拷位为ms
static uint8_t s_eeprom_save_pending = 1; // EEPROM 延迟保存待处理标志
uint16_t current_time;

//<<AICUBE_USER_GLOBAL_DEFINE_END>>



////////////////////////////////////////
// 项目主函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void main(void)
{
    //<<AICUBE_USER_MAIN_INITIAL_BEGIN>>
    // 锟节达拷锟斤拷锟斤拷锟矫伙拷锟斤拷锟斤拷锟斤拷锟斤拷始锟斤拷锟斤拷锟斤拷  
    //<<AICUBE_USER_MAIN_INITIAL_END>>

    SYS_Init();

    //<<AICUBE_USER_MAIN_CODE_BEGIN>>
    // 锟节达拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷一锟轿碉拷锟矫伙拷锟斤拷锟斤拷
    //<<AICUBE_USER_MAIN_CODE_END>>

    while (1)
    {
        //<<AICUBE_USER_MAIN_LOOP_BEGIN>>
        // 锟节达拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟矫伙拷锟斤拷循锟斤拷锟斤拷锟斤拷  
        
        current_time = Get_SysTick(); // 获取系统运行时间，单位为ms

        Buzzer_Tick();

        Modbus_Poll();

        if ((uint16_t)(current_time - s_last_time_key) >= 10)
        {
            s_last_time_key = current_time; // 保存上次扫描时间

            Key_Task(); // 执行按键扫描任务
        }
        

        if ((uint16_t)(current_time - s_last_time_100ms) >= 100)
        {
            s_last_time_100ms = current_time; // 保存上次执行100ms任务的时间
            // 更新实时数据
            sys_ui.ui_data.real_time_current_mA = Current_Control_GetActualCurrent_mA();
            sys_ui.ui_data.real_time_voltage_mV = Voltage_Control_GetActualVoltage_mV();
            sys_ui.ui_data.real_time_temperature_C = Temperature_Control_GetActualTemperaturex100();

            Control_Protect(); // 执行保护逻辑 + 风扇调速
            Control_Tick(); // 执行控制逻辑
            UI_Task();  // 刷新显示

            // 开机1秒后延迟保存EEPROM（避开启动时电源不稳定窗口）
            if (s_eeprom_save_pending && current_time > 1000) {
                s_eeprom_save_pending = 0;
                EEPROM_DelayedSave();
            }
        }

        //<<AICUBE_USER_MAIN_LOOP_END>>
    }
}

////////////////////////////////////////
// 系统初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void SYS_Init(void)
{
    EnableAccessXFR();                  //使能访问扩展XFR

    //<<AICUBE_USER_PREINITIAL_CODE_BEGIN>>
    // 
    //<<AICUBE_USER_PREINITIAL_CODE_END>>

    P0M0 = 0x00; P0M1 = 0x00;           //初始化P0口为准双向口模式
    P1M0 = 0x00; P1M1 = 0x00;           //初始化P1口为准双向口模式
    P2M0 = 0x00; P2M1 = 0x00;           //初始化P2口为准双向口模式
    P3M0 = 0x00; P3M1 = 0x00;           //初始化P3口为准双向口模式
    P4M0 = 0x00; P4M1 = 0x00;           //初始化P4口为准双向口模式
    P5M0 = 0x00; P5M1 = 0x00;           //初始化P5口为准双向口模式
    P6M0 = 0x00; P6M1 = 0x00;           //初始化P6口为准双向口模式
    P7M0 = 0x00; P7M1 = 0x00;           //初始化P7口为准双向口模式

    PORT0_Init();                       //P0口初始化
    PORT1_Init();                       //P1口初始化
    PORT2_Init();                       //P2口初始化
    PORT3_Init();                       //P3口初始化
    CLK_Init();                         //时钟模块初始化
    TIMER0_Init();                      //定时器0初始化
    TIMER1_Init();                      //定时器1初始化
    TIMER4_Init();                      //定时器4初始化
    UART1_Init();                       //串口1初始化
    UART3_Init();                       //串口3初始化
    ADC_Init();                         //ADC初始化
    PWM_Init();                         //增强型PWM初始化

    //<<AICUBE_USER_INITIAL_CODE_BEGIN>>
    // 锟节达拷锟斤拷锟斤拷锟矫伙拷锟斤拷始锟斤拷锟斤拷锟斤拷  
	
    Current_Control_Init();                    // 初始化电流控制模块

    Fan_Init();                                // 初始化风扇软件PWM

	OLED_Init();

    Buzzer_Init();
	
    UI_Init();
    
    EEPROM_Restore();                    // 从EEPROM加载参数（掉电保护恢复）

    Modbus_Init();

    Buzzer_PowerOn();                  // 开机提示音

    //<<AICUBE_USER_INITIAL_CODE_END>>

    EnableGlobalInt();                  //使能全局中断
}

////////////////////////////////////////
// 微秒延时函数
// 入口参数: us (设置延时的微秒值)
// 函数返回: 无
////////////////////////////////////////
void delay_us(uint16_t us)
{
    do
    {
        NOP(1);                         //(MAIN_Fosc + 500000) / 1000000 - 10
    } while (--us);
}


////////////////////////////////////////
// 毫秒延时函数
// 入口参数: ms (设置延时的毫秒值)
// 函数返回: 无
////////////////////////////////////////
void delay_ms(uint16_t ms)
{
    uint16_t i;

    do
    {
        i = MAIN_Fosc / 10000;
        while (--i);
    } while (--ms);
}


//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 锟节达拷锟斤拷锟斤拷锟矫伙拷锟斤拷锟斤拷实锟街达拷锟斤拷  
//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


