//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: uart.h
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#ifndef __UART_H__
#define __UART_H__


//<<AICUBE_USER_DEFINE_BEGIN>>
// 在此添加用户宏定义  
//<<AICUBE_USER_DEFINE_END>>


#define UART3_RXSIZE            16      //串口3接收缓冲区大小


void UART1_Init(void);
void UART1_SendByte(uint8_t dat);
void UART1_SendBuffer(uint8_t *dat, uint8_t size);
void UART1_SendString(char *str);
void UART3_Init(void);
void UART3_SendByte(uint8_t dat);
void UART3_SendBuffer(uint8_t *dat, uint8_t size);
void UART3_SendString(char *str);
uint16_t MODBUS_CRC16_Check(uint8_t *dat, int size);

extern BOOL fUART3SendBusy;
extern BOOL fUART3ReadyRead;
extern uint8_t u8UART3RxCount;
extern uint8_t xdata pu8UART3RxBuffer[UART3_RXSIZE];


//<<AICUBE_USER_EXTERNAL_DECLARE_BEGIN>>
// 在此添加用户外部函数和外部变量声明  
//<<AICUBE_USER_EXTERNAL_DECLARE_END>>


#endif
