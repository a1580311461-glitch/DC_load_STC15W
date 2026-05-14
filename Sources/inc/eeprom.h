#ifndef	__EEPROM_H
#define	__EEPROM_H

#include "def.h"


/* EEPROM 存储结构体 — 用于参数掉电保护 */
#define EEPROM_Lenth    16

typedef struct {
    uint8_t  modbusAddr;        /* Modbus地址 (1-247)                      */
    uint8_t  RunMode;           /* 运行模式: OP_MODE_CC=0, CR=1, CP=2     */
    uint16_t CC_I;              /* CC模式设定电流 (mA)                     */
    uint16_t CR_R;              /* CR模式设定电阻 (mΩ)                    */
    uint16_t CP_P;              /* CP模式设定功率 (mW)                    */
    uint16_t BootCNT;           /* 启动次数                               */
    uint16_t MaxVoltage_mV;     /* OVP阈值 (mV)                           */
    uint16_t MaxTemp_C;         /* OTP阈值 (°C)                          */
    uint8_t  end;               /* 结束标志 (0x00)                        */
} EEPROM_MyDataTypeDef;


void	DisableEEPROM(void);
void 	EEPROM_read_n(uint16_t EE_address, uint8_t *DataAddress, uint16_t number);
void 	EEPROM_write_n(uint16_t EE_address, uint8_t *DataAddress, uint16_t number);
void	EEPROM_SectorErase(uint16_t EE_address);
void	EEPROM_Restore(void);
void	EEPROM_Save(uint8_t isRestoreFactory);
void	EEPROM_DelayedSave(void);


#endif