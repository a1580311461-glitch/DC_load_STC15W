#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "def.h"

// Magic + version for EEPROM struct validation
#define SETTINGS_MAGIC      0xA5
#define SETTINGS_VERSION    0x01

typedef struct {
    uint8_t  magic;                     // 0xA5 — 标识已初始化
    uint8_t  version;                   // 0x01 — 结构版本
    uint8_t  op_mode;                   // CC=0, CR=1, CP=2
    uint8_t  device_address;            // RS485 地址 1~247
    uint16_t setting_current_mA;        // Target I (mA)
    uint16_t setting_resistance_mOhm;   // Target R (mOhm)
    uint16_t setting_power_mW;          // Target P (mW)
    uint16_t max_voltage_mV;            // OVP 阈值 (mV)
    uint16_t max_temperature_C;         // OTP 阈值 (°C)
    uint16_t checksum;                  // 前述所有字节异或和
} SettingsEeprom_t;

uint8_t Settings_Load(void);    // 返回 1=成功, 0=使用默认值
void Settings_Save(void);

#endif
