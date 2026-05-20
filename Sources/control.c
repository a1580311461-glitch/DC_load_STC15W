#include "config.h"

// 风扇软件PWM阈值（基于 FAN_PWM_PERIOD=10 的0~10刻度）
#define FAN_DUTY_ON    3       // 最低启动占空比 ≈ 30%

uint8_t output_en_state = 0;  // 软件跟踪OUTPUT_EN状态（P33开漏模式，读引脚不可靠）
static uint16_t xdata s_last_vcc_mv = 0;
static uint16_t xdata s_current_pwm_duty = 0;
static uint16_t xdata s_target_pwm_duty = 0;
static uint8_t xdata s_stepping_active = 0;
static uint16_t xdata s_closed_loop_target_mA = 0; // CR/CP闭环微调的目标电流
static uint8_t xdata s_fan_active = 0;             // 风扇迟滞状态跟踪

void Current_Control_Init(void)
{
    OUTPUT_EN_OFF(); // 初始关闭mosfet驱动输出
    LED_OUT_OFF(); // 初始状态输出指示灯灭

    s_current_pwm_duty = 100;          // 初始化当前占空比为100
    PWM2_SetWidth(s_current_pwm_duty); // 设置初始占空比
}

uint16_t Current_Control_GetActualCurrent_mA(void)
{
    // ADC_CH_CURRENT = 1 (P1.1)
    uint16_t voltage_mV = ADC_Convert_mV(1);

    return (uint32_t)voltage_mV * 100 / AMP_GAIN / R_SENSEx100; // 还原到 mA
}

void Current_Control_SetTarget(uint16_t target_mA)
{
    uint16_t vcc_mv = ADC_Get_Vcc();
    uint16_t new_duty;

    if (vcc_mv == 0)
    {
        vcc_mv = s_last_vcc_mv; // 用上次有效值
        if (vcc_mv == 0)
            return; // 首次无效直接跳过
    }

    s_last_vcc_mv = vcc_mv; // 更新上次有效VCC值

    new_duty = (uint32_t)target_mA * R_SENSEx100 * AMP_GAIN * MAX_PWM_DUTY / vcc_mv / 100;
    if (new_duty == s_target_pwm_duty)
        return; // 同值忽略

    s_target_pwm_duty = new_duty;

    // printf("\t\n %d -> PWM Duty: %d", s_current_pwm_duty, s_target_pwm_duty);

    s_stepping_active = 1; // 启动步进
}

// 步进式设定目标输出电流
void Current_Control_Tick(void)
{
    uint16_t actual_mA;
    int16_t error;

    if (s_stepping_active)
    {
        // 大步进逼近
        if (s_current_pwm_duty < s_target_pwm_duty)
        {
            if ((s_target_pwm_duty - s_current_pwm_duty) < PWM_STEP_SIZE)
                s_current_pwm_duty = s_target_pwm_duty;
            else
                s_current_pwm_duty += PWM_STEP_SIZE;
        }
        else if (s_current_pwm_duty > s_target_pwm_duty)
        {
            if ((s_current_pwm_duty - s_target_pwm_duty) < PWM_STEP_SIZE)
                s_current_pwm_duty = s_target_pwm_duty;
            else
                s_current_pwm_duty -= PWM_STEP_SIZE;
        }

        PWM2_SetWidth(s_current_pwm_duty);

        if (s_current_pwm_duty == s_target_pwm_duty)
            s_stepping_active = 0;
        return;
    }

// 增量闭环微调（使用 s_closed_loop_target_mA，支持 CR/CP 模式）
#define CFG_DEADBAND_MA 10 // ±10mA 死区

    actual_mA = Current_Control_GetActualCurrent_mA();
    error = (int16_t)s_closed_loop_target_mA - (int16_t)actual_mA;

    if (error > CFG_DEADBAND_MA)
    {
        // 电流偏小 → 加一点
        if (s_current_pwm_duty < MAX_PWM_DUTY)
            s_current_pwm_duty++;
    }
    else if (error < -CFG_DEADBAND_MA)
    {
        // 电流偏大 → 减一点
        if (s_current_pwm_duty > 0)
            s_current_pwm_duty--;
    }
    // 死区内不动，防抖

    PWM2_SetWidth(s_current_pwm_duty);
}

void Current_Control_ResetDuty(void)
{
    s_current_pwm_duty = 0;
    s_target_pwm_duty = 0;
    s_stepping_active = 0;
    PWM2_SetWidth(0);
}

uint16_t Voltage_Control_GetActualVoltage_mV(void)
{
    // ADC_CH_VOLTAGE = 0 (P1.0)
    uint16_t voltage_mV = ADC_Convert_mV(0);

    return (uint32_t)voltage_mV * 2; // 电压分压回路，实际电压是测量值的2倍
}

uint16_t Temperature_Control_GetActualTemperaturex100(void)
{
    // ADC_T = P1.2 = channel 2
    uint16_t raw = ADC_Convert(2);
    int t = NTC_GetTemperature(raw);

    // 返回 °C×100，超量程钳位
    if (t < NTC_TEMP_MIN_x100)
    {
        t = NTC_TEMP_MIN_x100;
    }

    if (t > NTC_TEMP_MAX_x100)
    {
        t = NTC_TEMP_MAX_x100;
    }

    return (uint16_t)t; // 只接正值显示, 实际-20°C以下概率极低
}


void Control_Tick(void)
{
    uint16_t target_current_mA;
    uint16_t voltage_mV;
    
    switch (sys_ui.ui_data.op_mode) {
    case OP_MODE_CC:
        target_current_mA = sys_ui.ui_data.setting_current_mA;
        break;
    case OP_MODE_CR:
        voltage_mV = Voltage_Control_GetActualVoltage_mV();
        // I = V / R → target = voltage_mV / (R_set / 1000)
        // R_set 单位 mΩ，voltage_mV / (R_set / 1000) = voltage_mV * 1000 / R_set
        target_current_mA = (uint32_t)voltage_mV * 1000 / sys_ui.ui_data.setting_resistance_mOhm;
        break;
    case OP_MODE_CP:
        voltage_mV = Voltage_Control_GetActualVoltage_mV();
        if (voltage_mV < 100) { target_current_mA = 0; break; } // 防除零
        // P = V * I → I = P / V
        target_current_mA = (uint32_t)sys_ui.ui_data.setting_power_mW * 1000 / voltage_mV;
        break;
    }
    
    // 统一限幅到 max_current_mA
    if (target_current_mA > sys_ui.ui_data.max_current_mA)
        target_current_mA = sys_ui.ui_data.max_current_mA;

    s_closed_loop_target_mA = target_current_mA;       // 保存给闭环微调用（CR/CP模式依赖此值）
    Current_Control_SetTarget(target_current_mA);       // 计算目标占空比（即使输出关闭也计算，以便打开时步进起点正确）

    // 输出关闭时不执行 PWM 步进和闭环微调，防止闭环读到 actual=0 误增占空比
    if (OUTPUT_EN_IS_ON) {
        Current_Control_Tick();                         // 执行 PWM 调节
    }
}


void Control_Protect(void)
{
    uint16_t current_mA = sys_ui.ui_data.real_time_current_mA;
    uint16_t temp_x100  = sys_ui.ui_data.real_time_temperature_C;
    uint16_t temp_C     = temp_x100 / 100;

    // === 过压保护 ===
    if (sys_ui.ui_data.real_time_voltage_mV > sys_ui.ui_data.max_voltage_mV)
    {
        sys_ui.ui_data.is_over_voltage = 1;
        OUTPUT_EN_OFF();
        LED_ERR_ON(); // 过压时点亮错误指示灯
        LED_OUT_OFF(); // 过压时关闭输出指示灯
        Buzzer_SetAlarm(1);
        PWM2_SetWidth(0);
        return;
    }

    // === 过温保护 (OTP) ===
    if (temp_C >= sys_ui.ui_data.max_temperature_C)
    {
        sys_ui.ui_data.is_over_temperature = 1;
        OUTPUT_EN_OFF();            // 关闭输出
        LED_ERR_ON(); // 过温时点亮错误指示灯
        LED_OUT_OFF(); // 过温时关闭输出指示灯
        Buzzer_SetAlarm(1);
        Fan_SetDuty(FAN_DUTY_MAX);   // 风扇满速降温
        return;
    }

    // === 风扇控制：温度/电流联合启停 + 动态调速（迟滞防抖） ===
    // 负载调整率测试时关闭风扇，避免PWM噪声干扰ADC测量精度
    if (sys_ui.ui_data.auto_test_running) {
        if (s_fan_active) {
            s_fan_active = 0;
            Fan_SetDuty(0);
        }
        return;
    }
    #define FAN_ON_TEMP_C       35      // 风扇启动温度阈值 (°C)
    #define FAN_OFF_TEMP_C      30      // 风扇关断温度迟滞 (°C)
    #define FAN_ON_CURRENT_MA   1000    // 风扇启动电流阈值 (mA)
    #define FAN_OFF_CURRENT_MA  300     // 风扇关断电流迟滞 (mA)

    {
        uint8_t fan_duty = 0;

        if (s_fan_active) {
            // 风扇已运行：两个条件都低于迟滞阈值才关闭
            if (temp_C >= FAN_OFF_TEMP_C || current_mA >= FAN_OFF_CURRENT_MA) {
                goto calc_fan_speed;
            } else {
                s_fan_active = 0;
            }
        } else {
            // 风扇未运行：温度或电流任一达到阈值即开启
            if (temp_C >= FAN_ON_TEMP_C || current_mA >= FAN_ON_CURRENT_MA) {
                s_fan_active = 1;
                goto calc_fan_speed;
            }
        }

        Fan_SetDuty(0);
        return;

    calc_fan_speed:
    {   
        // 温度分量：35°C→FAN_DUTY_ON(3), max_temperature_C→FAN_DUTY_MAX(10)
        uint8_t speed_by_temp = FAN_DUTY_ON;
        // 功率分量：实际功率/最大功率 × FAN_DUTY_MAX
        uint8_t speed_by_power = 0;
        if (temp_C > FAN_ON_TEMP_C && temp_C < sys_ui.ui_data.max_temperature_C) {
            speed_by_temp += (uint8_t)((uint32_t)(temp_C - FAN_ON_TEMP_C)
                            * (FAN_DUTY_MAX - FAN_DUTY_ON)
                            / (sys_ui.ui_data.max_temperature_C - FAN_ON_TEMP_C));
        }
        
        {
            uint32_t power_mW = (uint32_t)current_mA
                * sys_ui.ui_data.real_time_voltage_mV / 1000;
            uint32_t max_power_mW = (uint32_t)sys_ui.ui_data.max_current_mA
                * sys_ui.ui_data.max_voltage_mV / 1000;

            if (max_power_mW > 0)
                speed_by_power = (uint8_t)(power_mW * FAN_DUTY_MAX / max_power_mW);
        }

        // 取较大值作为最终转速
        fan_duty = (speed_by_temp > speed_by_power) ? speed_by_temp : speed_by_power;
        if (fan_duty > FAN_DUTY_MAX) fan_duty = FAN_DUTY_MAX;
        if (fan_duty < FAN_DUTY_ON)  fan_duty = FAN_DUTY_ON;
    }

        Fan_SetDuty(fan_duty);
    }
}
