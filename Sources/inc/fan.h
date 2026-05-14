#ifndef __FAN_H__
#define __FAN_H__

// 软件PWM参数：基于 Timer4 1ms 滴答
#define FAN_PWM_PERIOD  10      // PWM周期(ms)，100Hz，10级调速(0~10)

#define FAN_DUTY_MAX    FAN_PWM_PERIOD
#define FAN_DUTY_MIN    0

// 声明外部变量，供定时器中断直接使用
extern volatile uint8_t fan_duty;
extern volatile uint8_t fan_cnt;

void Fan_Init(void);
void Fan_SetDuty(uint8_t duty);         // 0=停止, FAN_PWM_PERIOD=全速

#endif
