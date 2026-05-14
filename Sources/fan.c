#include "config.h"

volatile uint8_t fan_duty = 0;
volatile uint8_t fan_cnt = 0;

void Fan_Init(void)
{
    FAN = 0;
    fan_duty = 0;
    fan_cnt = 0;
}

void Fan_SetDuty(uint8_t duty)
{
    if (duty > FAN_PWM_PERIOD) duty = FAN_PWM_PERIOD;
    fan_duty = duty;
    // 边界情况立即响应
    if (duty == 0) {
        FAN = 0;
    } else if (duty >= FAN_PWM_PERIOD) {
        FAN = 1;
    }
}
