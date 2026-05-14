#include "config.h"


static uint8_t xdata u8TxBuffer[MODBUS_FRAME_SIZE_MAX];  // 发送缓冲
static uint8_t *pu8RxFrame;       // 指向当前接收帧( = pu8UART3RxBuffer + 0 )
static uint8_t u8RxFrameLen;      // 帧长度( = u8UART3RxCount )
static uint16_t u16ModbusTimer;   // 上次收到字节时的 sys_tick 值


static void Modbus_ReadHoldingRegisters(void);
static void Modbus_WriteSingleRegister(void);
static void Modbus_WriteMultipleRegisters(void);
static void Modbus_SendException(uint8_t func, uint8_t ex_code);
static void Modbus_SendResponse(uint8_t len);


// 寄存器读函数
static uint16_t Modbus_ReadRegister(uint16_t addr) {
    switch (addr) {
        case MODBUS_REG_DEVICE_ADDR: 
            return sys_ui.ui_data.device_address;
        case MODBUS_REG_WORK_MODE: 
            return sys_ui.ui_data.op_mode;
        case MODBUS_REG_SETTING_CURRENT: 
            return sys_ui.ui_data.setting_current_mA;
        case MODBUS_REG_SETTING_RESISTANCE: 
            return sys_ui.ui_data.setting_resistance_mOhm;
        case MODBUS_REG_SETTING_POWER: 
            return sys_ui.ui_data.setting_power_mW;
        case MODBUS_REG_MAX_CURRENT: 
            return sys_ui.ui_data.max_current_mA;
        case MODBUS_REG_MAX_VOLTAGE: 
            return sys_ui.ui_data.max_voltage_mV;
        case MODBUS_REG_MAX_TEMPERATURE: 
            return sys_ui.ui_data.max_temperature_C;
        case MODBUS_REG_REAL_TIME_VOLTAGE: 
            return sys_ui.ui_data.real_time_voltage_mV;
        case MODBUS_REG_REAL_TIME_CURRENT: 
            return sys_ui.ui_data.real_time_current_mA;
        case MODBUS_REG_REAL_TIME_TEMPERATURE: 
            return sys_ui.ui_data.real_time_temperature_C;
        case MODBUS_REG_OUTPUT_STATE: 
            return OUTPUT_EN_IS_ON ? 1 : 0;
        case MODBUS_REG_OVERVOLTAGE_FLAG: 
            return sys_ui.ui_data.is_over_voltage;
        case MODBUS_REG_OVERTEMP_FLAG: 
            return sys_ui.ui_data.is_over_temperature;
        case MODBUS_REG_BOOT_COUNT:
            return sys_ui.ui_data.boot_count;
        default: 
            return 0xFFFF; // 非法地址返回0xFFFF
    }
}


// 寄存器写函数（1 成功/0 失败）
static uint8_t Modbus_WriteRegister(uint16_t addr, uint16_t value) {
    switch (addr) {
        case MODBUS_REG_DEVICE_ADDR: 
            if (value >= 1 && value <= 247) {
                sys_ui.ui_data.device_address = value;
                return 1; // 成功
            } else {
                return 0; // 非法值，失败
            }
        case MODBUS_REG_WORK_MODE: 
            if (value <= OP_MODE_CP) {
                sys_ui.ui_data.op_mode = (OpMode_e)value;
                return 1;
            } else {
                return 0; 
            }
        case MODBUS_REG_SETTING_CURRENT:
            if (value <= sys_ui.ui_data.max_current_mA) {
                sys_ui.ui_data.setting_current_mA = value;
                return 1;
            } else {
                return 0;
            }
        case MODBUS_REG_SETTING_RESISTANCE: 
            sys_ui.ui_data.setting_resistance_mOhm = value;
            return 1;
        case MODBUS_REG_SETTING_POWER: 
            sys_ui.ui_data.setting_power_mW = value;
            return 1;
        case MODBUS_REG_MAX_CURRENT: 
            sys_ui.ui_data.max_current_mA = value;
            return 1;
        case MODBUS_REG_MAX_VOLTAGE: 
            sys_ui.ui_data.max_voltage_mV = value;
            return 1;
        case MODBUS_REG_MAX_TEMPERATURE: 
            sys_ui.ui_data.max_temperature_C = value;
            return 1;
        case MODBUS_REG_OUTPUT_STATE:
            if (value == 0) {
                OUTPUT_EN_OFF();
                LED_OUT_OFF(); // 同步关闭LED指示
                return 1;
            } else if (value == 1) {
                OUTPUT_EN_ON();
                LED_OUT_ON(); // 同步打开LED指示
                return 1;
            } else {
                return 0;
            }
        default: 
            return 0; // 非法地址，失败
    }
}


// 发送正常响应函数
static void Modbus_SendResponse(uint8_t len) {
    LED_485_TOGGLE();         // 发送前翻转 485 指示灯
    RS485_SET_TX();
    UART3_SendBuffer(u8TxBuffer, len);
    while (fUART3SendBusy); // 等最后一字节硬件发完
    RS485_SET_RX();
}


// 发送异常响应函数
// 参数: func (原始功能码), ex_code (异常码)
// 响应格式： [地址] [func | 0x80] [ex_code] [CRC 低] [CRC 高]
static void Modbus_SendException(uint8_t func, uint8_t ex_code) {
    uint16_t crc;

    u8TxBuffer[0] = sys_ui.ui_data.device_address;
    u8TxBuffer[1] = func | 0x80;
    u8TxBuffer[2] = ex_code;
    crc = MODBUS_CRC16_Check(u8TxBuffer, 3);
    u8TxBuffer[3] = crc & 0xFF;       // CRC 低字节在前
    u8TxBuffer[4] = (crc >> 8) & 0xFF; // CRC 高字节在后
    Modbus_SendResponse(5);
}


static void Modbus_ReadHoldingRegisters(void) {
    uint16_t start_addr;
    uint16_t quantity;
    uint16_t end_addr;
    uint8_t addr_valid = 0;
    uint8_t i;
    uint16_t crc;
    uint8_t len;
    
    start_addr = (pu8RxFrame[2] << 8) | pu8RxFrame[3]; // 提取起始地址
    quantity = (pu8RxFrame[4] << 8) | pu8RxFrame[5];   // 提取数量

    // 限制单次最多读 20 个寄存器
    if (quantity < 1 || quantity > 20) {
        Modbus_SendException(0x03, MODBUS_EX_ILLEGAL_VALUE);
        return;
    }

    end_addr = start_addr + quantity - 1;
    if (start_addr >= 0x0000 && end_addr <= 0x0007) {
        addr_valid = 1;  // R/W 区
    }
    else if (start_addr >= 0x1000 && end_addr <= 0x1006) {
        addr_valid = 1;  // R/O 区
    }

    if (!addr_valid) {
        Modbus_SendException(0x03, MODBUS_EX_ILLEGAL_ADDR);
        return;
    }

    u8TxBuffer[0] = pu8RxFrame[0];
    u8TxBuffer[1] = 0x03;
    u8TxBuffer[2] = quantity * 2;  // 字节数
    // 循环填充每个寄存器的数据，大端格式
    for (i = 0; i < quantity; i++) {
        uint16_t value = Modbus_ReadRegister(start_addr + i);
        u8TxBuffer[3 + i*2]     = (value >> 8) & 0xFF;   // 高字节
        u8TxBuffer[3 + i*2 + 1] = value & 0xFF;           // 低字节
    }

    // 计算 CRC 并追加到数据后面
    len = 3 + quantity * 2;
    crc = MODBUS_CRC16_Check(u8TxBuffer, len);
    u8TxBuffer[len]     = crc & 0xFF;
    u8TxBuffer[len + 1] = (crc >> 8) & 0xFF;

    Modbus_SendResponse(len + 2);  // 头+数据+CRC
    
}


static void Modbus_WriteSingleRegister(void) {
    uint16_t reg_addr = (pu8RxFrame[2] << 8) | pu8RxFrame[3];
    uint16_t value    = (pu8RxFrame[4] << 8) | pu8RxFrame[5];
    uint16_t crc;

    if (Modbus_WriteRegister(reg_addr, value)) {
        // 成功 → 原样复制请求帧作为响应（6 字节 + 追 CRC）
        u8TxBuffer[0] = pu8RxFrame[0];
        u8TxBuffer[1] = pu8RxFrame[1];
        u8TxBuffer[2] = pu8RxFrame[2];
        u8TxBuffer[3] = pu8RxFrame[3];
        u8TxBuffer[4] = pu8RxFrame[4];
        u8TxBuffer[5] = pu8RxFrame[5];
        crc = MODBUS_CRC16_Check(u8TxBuffer, 6);
        u8TxBuffer[6] = crc & 0xFF;
        u8TxBuffer[7] = (crc >> 8) & 0xFF;
        Modbus_SendResponse(8);
    } else {
        Modbus_SendException(0x06, MODBUS_EX_ILLEGAL_VALUE);
    }
}


static void Modbus_WriteMultipleRegisters(void) {
    uint16_t start_addr = (pu8RxFrame[2] << 8) | pu8RxFrame[3];
    uint16_t quantity   = (pu8RxFrame[4] << 8) | pu8RxFrame[5];
    uint8_t  byte_count = pu8RxFrame[6];
    uint8_t  i;
    uint16_t crc;

    // 验证请求合法性
    if (quantity < 1 || quantity > 8) {
        Modbus_SendException(0x10, MODBUS_EX_ILLEGAL_VALUE);
        return;
    }
    if (byte_count != quantity * 2) {
        Modbus_SendException(0x10, MODBUS_EX_ILLEGAL_VALUE);
        return;
    }
    // 只能写 R/W 区（0x0000~0x0007）
    if (start_addr < 0x0000 || (start_addr + quantity - 1) > 0x0007) {
        Modbus_SendException(0x10, MODBUS_EX_ILLEGAL_ADDR);
        return;
    }

    // 逐个写入
    for (i = 0; i < quantity; i++) {
        uint16_t value = (pu8RxFrame[7 + i*2] << 8) | pu8RxFrame[7 + i*2 + 1];
        if (!Modbus_WriteRegister(start_addr + i, value)) {
            Modbus_SendException(0x10, MODBUS_EX_ILLEGAL_VALUE);
            return;
        }
    }

    // 响应：addr + 0x10 + 起始地址(2) + 数量(2) + CRC
    u8TxBuffer[0] = pu8RxFrame[0];
    u8TxBuffer[1] = 0x10;
    u8TxBuffer[2] = pu8RxFrame[2];
    u8TxBuffer[3] = pu8RxFrame[3];
    u8TxBuffer[4] = pu8RxFrame[4];
    u8TxBuffer[5] = pu8RxFrame[5];
    crc = MODBUS_CRC16_Check(u8TxBuffer, 6);
    u8TxBuffer[6] = crc & 0xFF;
    u8TxBuffer[7] = (crc >> 8) & 0xFF;
    Modbus_SendResponse(8);
}


// 帧校验 + 分发
static void Modbus_ProcessFrame(void) {
    uint8_t func;
    uint16_t crc;

    // 帧长度合法性：至少 4 字节（地址+功能码+CRC(2)）
    if (u8RxFrameLen < 4) {
        return; // 帧太短，丢弃
    }

    // CRC 校验
    crc = MODBUS_CRC16_Check(pu8RxFrame, u8RxFrameLen);
    if (crc != 0) {
        return; // CRC 错误，丢弃
    }

    // 地址检查
    if (pu8RxFrame[0] != sys_ui.ui_data.device_address) {
        return;
    }

    // 功能码分发
    func = pu8RxFrame[1];
    switch (func) {
        case MODBUS_FC_READ_HOLDING:   // 0x03
            Modbus_ReadHoldingRegisters();
            break;
        case MODBUS_FC_WRITE_SINGLE:   // 0x06
            Modbus_WriteSingleRegister();
            break;
        case MODBUS_FC_WRITE_MULTIPLE: // 0x10
            Modbus_WriteMultipleRegisters();
            break;
        default:
            Modbus_SendException(func, MODBUS_EX_ILLEGAL_FUNC);
            break;
    }
}


// Modbus 初始化函数
void Modbus_Init(void) {
    LED_485_OFF(); // 初始状态 485 指示灯灭
    u16ModbusTimer = Get_SysTick();  // 初始化时间戳
    u8UART3RxCount = 0;             // 清空接收计数
    fUART3ReadyRead = 0;            // 清空标志
}


void Modbus_Poll(void) {
    // 有新字节到达 → 刷新时间戳
    if (fUART3ReadyRead) {
        fUART3ReadyRead = 0;
        u16ModbusTimer = Get_SysTick();
    }
    
    // 缓冲区有数据且超时 4ms → 帧结束
    if (u8UART3RxCount > 0) {
        if ((uint16_t)(Get_SysTick() - u16ModbusTimer) >= 4) {
            // 帧完成 → 暂存帧指针和长度
            pu8RxFrame = pu8UART3RxBuffer;
            u8RxFrameLen = u8UART3RxCount;
            
            Modbus_ProcessFrame();  // 处理这帧
            
            u8UART3RxCount = 0;    // 清空缓冲区准备接收下一帧
        }
    }
}

