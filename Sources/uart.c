//<<AICUBE_USER_HEADER_REMARK_BEGIN>>
////////////////////////////////////////
// 在此添加用户文件头说明信息  
// 文件名称: uart.c
// 文件描述: 
// 文件版本: V1.0
// 修改记录:
//   1. (2026-04-16) 创建文件
////////////////////////////////////////
//<<AICUBE_USER_HEADER_REMARK_END>>


#include "config.h"


//<<AICUBE_USER_INCLUDE_BEGIN>>
// 在此添加用户头文件包含  
//<<AICUBE_USER_INCLUDE_END>>


//<<AICUBE_USER_GLOBAL_DEFINE_BEGIN>>
// 在此添加用户全局变量定义、用户宏定义以及函数声明  
#define UART3_SetIntPriority(n)  // 定义一个空宏来消除AICube底库缺失造成的报错
//<<AICUBE_USER_GLOBAL_DEFINE_END>>


BOOL fUART3SendBusy;
BOOL fUART3ReadyRead;
uint8_t u8UART3RxCount;
uint8_t xdata pu8UART3RxBuffer[UART3_RXSIZE];

////////////////////////////////////////
// 串口1初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void UART1_Init(void)
{
#ifdef BAUDRATE
#undef BAUDRATE
#endif
#define BAUDRATE                (115200UL)
#define T2_RELOAD               (65536 - (SYSCLK / BAUDRATE + 2) / 4)

    UART1_SwitchP3031();                //设置串口数据端口: RxD (P3.0), TxD (P3.1)

    UART1_Timer2BRT();                  //选择定时器2作为串口1波特率发生器
    TIMER2_TimerMode();                 //设置定时器2为定时模式
    TIMER2_1TMode();                    //设置定时器2为1T模式
    TIMER2_SetReload16(T2_RELOAD);      //设置定时器2的16位重载值
    TIMER2_Run();                       //定时器2开始运行

    UART1_DisableRx();                  //禁止串口1接收数据
    UART1_Mode1();                      //设置串口1为模式1 (8位数据可变波特率)

    UART1_SetTxFlag();                  //设置发送标志以配合printf函数

    //<<AICUBE_USER_UART1_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  
    //<<AICUBE_USER_UART1_INITIAL_END>>
}

////////////////////////////////////////
// 重写printf字符发送重定向函数
// 入口参数: dat (printf函数待打印的字符)
// 函数返回: 需要返回入口参数的数据
////////////////////////////////////////
char putchar (char dat)                 //将串口1和printf函数绑定
{
    while (!UART1_CheckTxFlag());       //等待之前的数据发送完成
    UART1_ClearTxFlag();                //清除完成标志
    UART1_SendData(dat);                //发送当前字节

    return dat;
}

////////////////////////////////////////
// 串口1发送数据函数
// 入口参数: dat (待发送的字节数据)
// 函数返回: 无
////////////////////////////////////////
void UART1_SendByte(uint8_t dat)
{
    putchar((char)dat);
}

////////////////////////////////////////
// 串口1发送多字节数据函数
// 入口参数: dat  (发送数据缓冲区)
//           size (数据大小)
// 函数返回: 无
////////////////////////////////////////
void UART1_SendBuffer(uint8_t *dat, uint8_t size)
{
    while (size--)                      //判断数据是否结束
        UART1_SendByte(*dat++);         //发送当前字节
}

////////////////////////////////////////
// 串口1发送字符串函数
// 入口参数: str  (字符串首地址)
// 函数返回: 无
////////////////////////////////////////
void UART1_SendString(char *str)
{
    printf(str);                        //直接使用printf函数打印字符串
}

////////////////////////////////////////
// 串口3初始化函数
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void UART3_Init(void)
{
#ifdef BAUDRATE
#undef BAUDRATE
#endif
#define BAUDRATE                (9600UL)
#define T3_RELOAD               (65536 - (SYSCLK / BAUDRATE + 2) / 4)

    UART3_SwitchP0001();                //设置串口数据端口: RxD3 (P0.0), TxD3 (P0.1)

    UART3_Timer3BRT();                  //选择定时器3作为串口3波特率发生器
    TIMER3_TimerMode();                 //设置定时器3为定时模式
    TIMER3_1TMode();                    //设置定时器3为1T模式
    TIMER3_SetReload16(T3_RELOAD);      //设置定时器3的16位重载值
    TIMER3_Run();                       //定时器3开始运行

    UART3_EnableRx();                   //使能串口3接收数据
    UART3_Mode0();                      //设置串口3为模式0 (8位数据可变波特率)
    UART3_SetIntPriority(0);            //设置中断为低优先级
    UART3_EnableInt();                  //使能串口3中断

    fUART3SendBusy = 0;                 //清除发送数据忙标志
    fUART3ReadyRead = 0;                //清除接收数据完成标志

    //<<AICUBE_USER_UART3_INITIAL_BEGIN>>
    // 在此添加用户初始化代码  

    RS485_SET_RX();   // 初始化方向为接收

    //<<AICUBE_USER_UART3_INITIAL_END>>
}

////////////////////////////////////////
// 串口3发送数据函数
// 入口参数: dat (待发送的字节数据)
// 函数返回: 无
////////////////////////////////////////
void UART3_SendByte(uint8_t dat)
{
    while (fUART3SendBusy);             //等待之前的数据发送完成
    UART3_SendData(dat);                //发送当前字节
    fUART3SendBusy = 1;                 //设置发送忙标志
}

////////////////////////////////////////
// 串口3发送多字节数据函数
// 入口参数: dat  (发送数据缓冲区)
//           size (数据大小)
// 函数返回: 无
////////////////////////////////////////
void UART3_SendBuffer(uint8_t *dat, uint8_t size)
{
    while (size--)                      //判断数据是否结束
        UART3_SendByte(*dat++);         //发送当前字节
}

////////////////////////////////////////
// 串口3发送字符串函数
// 入口参数: str  (字符串首地址)
// 函数返回: 无
////////////////////////////////////////
void UART3_SendString(char *str)
{
    char c;

    while (c = *str++)                  //判断字符串是否结束
        UART3_SendByte(c);              //发送当前字节
}

////////////////////////////////////////
// 计算/校验MODBUS数据包的CRC16函数
// 基于(x^16 + x^15 + x^2 + 1)的MODBUS-CRC16多项式
// 入口参数: dat (待计算/校验的字节)
// 函数返回: 如果数据包中未包含CRC数据，则返回值为整包数据的CRC结果
//           如果数据包中已保护CRC数据，则返回值为0表示CRC正确，否则CRC错误
////////////////////////////////////////
uint16_t MODBUS_CRC16_Check(uint8_t *dat, int size)
{
    static uint16_t code CRC_TAB[256] = //使用查表法计算CRC
    {
        0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
        0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
        0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
        0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
        0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
        0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
        0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
        0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
        0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
        0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
        0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
        0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
        0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
        0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
        0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
        0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
        0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
        0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
        0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
        0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
        0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
        0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
        0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
        0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
        0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
        0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
        0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
        0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
        0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
        0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
        0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
        0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
    };
    uint16_t crc;                       //定义变量保存CRC结果
    int i;

    crc = 0xffff;                       //CRC结果初始化
    for (i = 0; i < size; i++)
        crc = (crc >> 8) ^ CRC_TAB[(crc & 0xff) ^ dat[i]]; //计算CRC

    return crc;                         //返回CRC结果
}


////////////////////////////////////////
// 串口3中断服务程序
// 入口参数: 无
// 函数返回: 无
////////////////////////////////////////
void UART3_ISR(void) interrupt UART3_VECTOR
{
    //<<AICUBE_USER_UART3_ISR_CODE1_BEGIN>>
    // 在此添加中断函数用户代码  
    if (UART3_CheckTxFlag())            //判断串口发送中断
    {
        UART3_ClearTxFlag();            //清除串口发送中断标志

        fUART3SendBusy = 0;             //清除发送忙标志
    }

    if (UART3_CheckRxFlag())            //判断串口接收中断
    {
        UART3_ClearRxFlag();            //清除串口接收中断标志

        pu8UART3RxBuffer[u8UART3RxCount++] = UART3_ReadData(); //保存接收的数据到接收缓冲区
        if (u8UART3RxCount >= UART3_RXSIZE)
            u8UART3RxCount = 0;         //防止数组边界溢出

        fUART3ReadyRead = 1;            //设置接收数据完成标志
    }
    //<<AICUBE_USER_UART3_ISR_CODE1_END>>
}


//<<AICUBE_USER_FUNCTION_IMPLEMENT_BEGIN>>
// 在此添加用户函数实现代码  
//<<AICUBE_USER_FUNCTION_IMPLEMENT_END>>


