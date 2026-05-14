#include "config.h"

/* EEPROM 联合体：结构体与字节数组覆盖，方便整块读写 */
static xdata union {
    EEPROM_MyDataTypeDef EEPROM_MyData;
    unsigned char cDate[EEPROM_Lenth];
} unionMyData;



//========================================================================
// 函数: void DisableEEPROM(void)
// 描述: 禁止访问IAP.
// 参数: non.
// 返回: non.
//========================================================================
void DisableEEPROM(void)
{
    IAP_CONTR = 0;          //禁止IAP操作
    IAP_CMD   = 0;          //去除IAP命令
    IAP_TRIG  = 0;          //防止IAP命令误触发
    IAP_ADDRH = 0xff;       //清0地址高字节
    IAP_ADDRL = 0xff;       //清0地址低字节，指向非EEPROM区，防止误操作
}

//========================================================================
// 函数: void EEPROM_read_n(u16 EE_address, u8 *DataAddress, u16 number)
// 描述: 从指定EEPROM首地址读出n个字节放指定的缓冲.
// 参数: EE_address:  读出EEPROM的首地址.
//       DataAddress: 读出数据放缓冲的首地址.
//       number:      读出的字节长度.
// 返回: non.
//========================================================================
void EEPROM_read_n(u16 EE_address, u8 *DataAddress, u16 number)
{
    EA = 0;     //禁止中断
    IAP_CONTR = IAPCON_IAPEN_MSK;       //使能IAP
    IAP_SetTimeBase();                  //设置等待时间
    do
    {
        IAP_CMD = 1;                    //读命令
        IAP_ADDRH = EE_address / 256;   //送地址高字节
        IAP_ADDRL = EE_address % 256;   //送地址低字节
        IAP_Trigger();                  //触发IAP操作
        _nop_();
        *DataAddress = IAP_DATA;        //读出的数据
        EE_address++;
        DataAddress++;
    } while(--number);

    DisableEEPROM();
    EA = 1;     //重新允许中断
}

//========================================================================
// 函数: void EEPROM_SectorErase(u16 EE_address)
// 描述: 把指定地址的EEPROM扇区擦除.
// 参数: EE_address:  要擦除的扇区EEPROM的地址.
// 返回: non.
//========================================================================
void EEPROM_SectorErase(u16 EE_address)
{
    EA = 0;     //禁止中断
    // 只有扇区擦除，没有字节擦除，512字节/扇区
    IAP_ADDRH = EE_address / 256;       //送扇区地址高字节
    IAP_ADDRL = EE_address % 256;       //送扇区地址低字节
    IAP_CONTR = IAPCON_IAPEN_MSK;       //使能IAP
    IAP_SetTimeBase();                  //设置等待时间
    IAP_CMD = 3;                        //扇区擦除命令
    IAP_Trigger();
    _nop_();
    DisableEEPROM();
    EA = 1;     //重新允许中断
}

//========================================================================
// 函数: void EEPROM_write_n(u16 EE_address, u8 *DataAddress, u16 number)
// 描述: 把缓冲的n个字节写入指定首地址的EEPROM.
// 参数: EE_address:  写入EEPROM的首地址.
//       DataAddress: 写入源数据的缓冲的首地址.
//       number:      写入的字节长度.
// 返回: non.
//========================================================================
void EEPROM_write_n(u16 EE_address, u8 *DataAddress, u16 number)
{
    EA = 0;     //禁止中断
    IAP_CONTR = IAPCON_IAPEN_MSK;       //使能IAP
    IAP_SetTimeBase();                  //设置等待时间
    do
    {
        IAP_ADDRH = EE_address / 256;   //送地址高字节
        IAP_ADDRL = EE_address % 256;   //送地址低字节
        IAP_DATA  = *DataAddress;       //送数据到IAP_DATA
        IAP_CMD = 2;                    //编程命令
        IAP_Trigger();
        _nop_();
        EE_address++;
        DataAddress++;
    } while(--number);

    DisableEEPROM();
    EA = 1;     //重新允许中断
}


/*------------------------------------------------------------------*/
/* 参数恢复：上电时从EEPROM加载运行参数                             */
/* 首次上电(modbusAddr==0xFF)自动恢复出厂设置                       */
/* 掉电保护核心：确保所有运行参数在掉电后不丢失                     */
/*------------------------------------------------------------------*/
void EEPROM_Restore(void)
{
    EEPROM_read_n(0, &unionMyData.cDate[0], EEPROM_Lenth);

    /* 初次启动或数据无效 → 恢复出厂设置 */
    if (unionMyData.EEPROM_MyData.modbusAddr == 0xFF)
    {
        EEPROM_Save(1);                     //写入出厂默认值
        delay_ms(10);
        EEPROM_read_n(0, &unionMyData.cDate[0], EEPROM_Lenth);
    }

    /* 将EEPROM数据映射到系统UI结构 */
    sys_ui.ui_data.op_mode              = (OpMode_e)unionMyData.EEPROM_MyData.RunMode;
    sys_ui.ui_data.setting_current_mA   = unionMyData.EEPROM_MyData.CC_I;
    sys_ui.ui_data.setting_resistance_mOhm = unionMyData.EEPROM_MyData.CR_R;
    sys_ui.ui_data.setting_power_mW     = unionMyData.EEPROM_MyData.CP_P;
    sys_ui.ui_data.device_address       = unionMyData.EEPROM_MyData.modbusAddr;
    sys_ui.ui_data.max_voltage_mV       = unionMyData.EEPROM_MyData.MaxVoltage_mV;
    sys_ui.ui_data.max_temperature_C    = unionMyData.EEPROM_MyData.MaxTemp_C;

    /* 更新启动次数 */
    unionMyData.EEPROM_MyData.BootCNT++;
    sys_ui.ui_data.boot_count = unionMyData.EEPROM_MyData.BootCNT;

    /* 钳制安全范围：OTP/OVP阈值过低说明数据损坏 */
    if (sys_ui.ui_data.max_temperature_C < 30)
        sys_ui.ui_data.max_temperature_C = 70;
    if (sys_ui.ui_data.max_voltage_mV < 1000)
        sys_ui.ui_data.max_voltage_mV = 5500;
    if (sys_ui.ui_data.setting_current_mA > 3000)
        sys_ui.ui_data.setting_current_mA = 1000;

    //EEPROM_Save(0);  //开机时不回写，避免电源波动损坏，由 EEPROM_DelayedSave 延迟执行
}

// 延迟保存启动计数（系统稳定后调用，避免开机时电源波动损坏EEPROM）
void EEPROM_DelayedSave(void)
{
    EEPROM_Save(0);
}


/*------------------------------------------------------------------*/
/* 参数存储：保存当前运行参数到EEPROM，实现掉电保护                 */
/* isRestoreFactory=1 写入出厂默认值; =0 保存当前RAM参数            */
/*------------------------------------------------------------------*/
void EEPROM_Save(unsigned char isRestoreFactory)
{
    if (isRestoreFactory)   /* 恢复出厂设置 */
    {
        unionMyData.EEPROM_MyData.modbusAddr    = 21;        //Modbus地址
        unionMyData.EEPROM_MyData.RunMode       = 0;          //OP_MODE_CC
        unionMyData.EEPROM_MyData.CC_I          = 1000;       //1.000 A
        unionMyData.EEPROM_MyData.CR_R          = 5000;       //5.000 Ω
        unionMyData.EEPROM_MyData.CP_P          = 2500;       //2.500 W
        unionMyData.EEPROM_MyData.BootCNT       = 0;          //启动次数（++后为1）
        unionMyData.EEPROM_MyData.MaxVoltage_mV = 5500;       //OVP 5.5V
        unionMyData.EEPROM_MyData.MaxTemp_C     = 70;         //OTP 70°C
    }
    else                /* 保存当前参数 */
    {
        unionMyData.EEPROM_MyData.modbusAddr    = (uint8_t)sys_ui.ui_data.device_address;
        unionMyData.EEPROM_MyData.RunMode       = (uint8_t)sys_ui.ui_data.op_mode;
        unionMyData.EEPROM_MyData.CC_I          = sys_ui.ui_data.setting_current_mA;
        unionMyData.EEPROM_MyData.CR_R          = sys_ui.ui_data.setting_resistance_mOhm;
        unionMyData.EEPROM_MyData.CP_P          = sys_ui.ui_data.setting_power_mW;
        unionMyData.EEPROM_MyData.BootCNT       = sys_ui.ui_data.boot_count;
        unionMyData.EEPROM_MyData.MaxVoltage_mV = sys_ui.ui_data.max_voltage_mV;
        unionMyData.EEPROM_MyData.MaxTemp_C     = sys_ui.ui_data.max_temperature_C;
    }

    unionMyData.EEPROM_MyData.end = 0x00;

    EEPROM_SectorErase(0);
    EEPROM_write_n(0, &unionMyData.cDate[0], EEPROM_Lenth);
}
