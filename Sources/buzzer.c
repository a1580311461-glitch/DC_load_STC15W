////////////////////////////////////////
// 文件名称: buzzer.c
// 文件描述: 无源蜂鸣器驱动模块，基于Timer1产生2kHz方波载波，
//           sys_tick(1ms)控制发声节奏包络，双层非阻塞架构
// 文件版本: V1.0
// 修改记录:
//   1. (2026-05-09) 创建文件
////////////////////////////////////////

#include "config.h"


// 发声节奏时间常量 (单位: ms)  
#define BUZZER_KEY_DURATION     20      // 按键提示音发声时间
#define BUZZER_TICK_DURATION    5       // 连按键反馈嘀嗒声时间
#define BUZZER_ALARM_ON_MS      100     // 报警音单次发声时间
#define BUZZER_ALARM_OFF_MS     100     // 报警音单次间歇时间
#define BUZZER_POWER_ON_MS      100     // 开机提示音发声时间
#define BUZZER_POWER_OFF_MS     100     // 开机提示音间歇时间


// 发声状态枚举
typedef enum {
    BUZZER_IDLE = 0,
    BUZZER_KEY_BEEP,
    BUZZER_SHORT_TICK,          // 短嘀嗒声(5ms)，连按数值变化反馈
    BUZZER_POWER_ON_1,          // 开机第1次发声
    BUZZER_POWER_ON_OFF,        // 开机间歇
    BUZZER_POWER_ON_2,          // 开机第2次发声
    BUZZER_ALARM_ON,            // 报警发声阶段
    BUZZER_ALARM_OFF            // 报警间歇阶段
} BuzzerState_e;

static BuzzerState_e xdata s_state = BUZZER_IDLE;
static uint16_t xdata s_tick_start = 0; // 状态机进入当前阶段的时刻(Get_SysTick())
static uint8_t xdata s_alarm_enable = 0;// 报警使能标志

volatile uint8_t buzzer_active = 0;   // =1时Timer1 ISR翻转BUZZER产生方波

////////////////////////////////////////
// 蜂鸣器初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void Buzzer_Init(void)
{
    BUZZER = 0;             // 初始化拉低引脚，保护NPN三极管
    buzzer_active = 0;
    s_alarm_enable = 0;
    s_state = BUZZER_IDLE;
}

////////////////////////////////////////
// 蜂鸣器状态机轮询函数（在主循环中调用，1ms精度）
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void Buzzer_Tick(void)
{
    uint16_t now;
    uint16_t elapsed;

    if (s_state == BUZZER_IDLE)
    {
        // IDLE状态下若报警使能，自动进入报警循环
        if (s_alarm_enable)
        {
            buzzer_active = 1;
            s_tick_start = Get_SysTick();
            s_state = BUZZER_ALARM_ON;
        }
        return;
    }

    now = Get_SysTick();
    elapsed = (uint16_t)(now - s_tick_start);

    switch (s_state)
    {
    case BUZZER_KEY_BEEP:
        if (elapsed >= BUZZER_KEY_DURATION)
        {
            buzzer_active = 0;
            s_state = BUZZER_IDLE;
        }
        break;

    case BUZZER_SHORT_TICK:
        if (elapsed >= BUZZER_TICK_DURATION)
        {
            buzzer_active = 0;
            s_state = BUZZER_IDLE;
        }
        break;

    case BUZZER_POWER_ON_1:
        if (elapsed >= BUZZER_POWER_ON_MS)
        {
            buzzer_active = 0;
            s_tick_start = now;
            s_state = BUZZER_POWER_ON_OFF;
        }
        break;

    case BUZZER_POWER_ON_OFF:
        if (elapsed >= BUZZER_POWER_OFF_MS)
        {
            buzzer_active = 1;
            s_tick_start = now;
            s_state = BUZZER_POWER_ON_2;
        }
        break;

    case BUZZER_POWER_ON_2:
        if (elapsed >= BUZZER_POWER_ON_MS)
        {
            buzzer_active = 0;
            s_state = BUZZER_IDLE;
        }
        break;

    case BUZZER_ALARM_ON:
        if (elapsed >= BUZZER_ALARM_ON_MS)
        {
            buzzer_active = 0;
            s_tick_start = now;
            s_state = BUZZER_ALARM_OFF;
        }
        break;

    case BUZZER_ALARM_OFF:
        if (elapsed >= BUZZER_ALARM_OFF_MS)
        {
            if (s_alarm_enable)
            {
                buzzer_active = 1;
                s_tick_start = now;
                s_state = BUZZER_ALARM_ON;
            }
            else
            {
                s_state = BUZZER_IDLE;
            }
        }
        break;

    default:
        s_state = BUZZER_IDLE;
        break;
    }
}

////////////////////////////////////////
// 按键提示音（非阻塞，单次发声 50ms）
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void Buzzer_KeyBeep(void)
{
    buzzer_active = 1;
    s_tick_start = Get_SysTick();
    s_state = BUZZER_KEY_BEEP;
}

////////////////////////////////////////
// 短促嘀嗒声（非阻塞，5ms短促发声，连按数值变化时使用）
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void Buzzer_ShortTick(void)
{
    buzzer_active = 1;
    s_tick_start = Get_SysTick();
    s_state = BUZZER_SHORT_TICK;
}

////////////////////////////////////////
// 开机提示音（非阻塞：响100ms → 停100ms → 响100ms）
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void Buzzer_PowerOn(void)
{
    buzzer_active = 1;
    s_tick_start = Get_SysTick();
    s_state = BUZZER_POWER_ON_1;
}

////////////////////////////////////////
// 报警音控制
// 入口参数: enable = 1 开启急促循环报警(响100ms停100ms)
//           enable = 0 立即停止报警并清空状态
// 函数返回: 无
////////////////////////////////////////
void Buzzer_SetAlarm(uint8_t enable)
{
    if (enable)
    {
        s_alarm_enable = 1;
        // 若当前空闲则立即启动报警
        if (s_state == BUZZER_IDLE)
        {
            buzzer_active = 1;
            s_tick_start = Get_SysTick();
            s_state = BUZZER_ALARM_ON;
        }
        // 非空闲状态：报警标志已置位，状态机回到IDLE后会自动切入报警
    }
    else
    {
        s_alarm_enable = 0;
        buzzer_active = 0;
        s_state = BUZZER_IDLE;
    }
}
