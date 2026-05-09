#ifndef __NTC_H
#define __NTC_H


// // 接线: [GND] --- [10k] --- ADC_In(P1.2) --- [NTC] --- [+5V]
// // 计算法

// // 热敏电阻参数 (MF52-103-3950)
// #define NTC_B               3950.0f     // B值(25/50)
// #define NTC_T0_KELVIN       298.15f     // 标称温度 25℃
// #define NTC_R0              10000.0f    // 标称电阻 10kΩ @25℃
// #define NTC_R_BALANCE       10000.0f    // 分压下偏置电阻 10kΩ


// float Temperature_byNTC(uint16_t adc_mV, uint16_t vcc_mV);


// 接线: [GND] --- [10k] --- ADC_In(P1.2) --- [NTC] --- [+5V]
// 查表法: 由 gen_ntc_table.py 生成

/* NTC温度查表 1024点
 * 入参: ADC原始值(0-1023), 来自 ADC_Convert(2)
 * 返回: 温度值 × 100 (int), 例如 25.67℃ → 2567
 *       超出量程返回 NTC_TEMP_MIN_x100 / NTC_TEMP_MAX_x100
 */
#define NTC_TEMP_MIN_x100   (-2000)     // -20.00℃
#define NTC_TEMP_MAX_x100   15000       // 150.00℃
#define NTC_TABLE_SIZE 1024

extern code int ntc_temp_table[NTC_TABLE_SIZE];

/* 查表得到温度值 */
#define NTC_GetTemperature(adc_raw) ((adc_raw) >= NTC_TABLE_SIZE ? NTC_TEMP_MAX_x100 : ntc_temp_table[adc_raw])



#endif
