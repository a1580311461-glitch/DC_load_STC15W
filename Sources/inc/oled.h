#ifndef __OLED_H
#define __OLED_H

//----OLED端口定义
//sbit OLED_SCL  = P2^1; // D0SCLK
//sbit OLED_SDIN = P2^0; // D1MOSI

#define IIC_SLAVE_ADDR 0x78  //IIC slave device address
//#define IIC_SLAVE_ADDR 0x3D  //IIC slave device address
//See datasheet for Address: 0x3D for 128x64, 0x3C for 128x32

#define SIZE        16
#define XLevelL     0x02
#define XLevelH     0x10
#define Max_Column  128
#define Max_Row     64
#define Brightness  0xFF
#define X_WIDTH     128
#define Y_WIDTH     64

#define OLED_CMD    0  //写命令
#define OLED_DATA   1  //写数据
#define OLED_MODE   0  //OLED模式选择, 0---4线串行模式, 1---并行8080模式

#define OLED_CS_Clr()  OLED_CS=0
#define OLED_CS_Set()  OLED_CS=1

#define OLED_RST_Clr() OLED_RST=0
#define OLED_RST_Set() OLED_RST=1

#define OLED_DC_Clr() OLED_DC=0
#define OLED_DC_Set() OLED_DC=1

#define OLED_SCLK_Clr() OLED_SCL=0
#define OLED_SCLK_Set() OLED_SCL=1

#define OLED_SDIN_Clr() OLED_SDIN=0
#define OLED_SDIN_Set() OLED_SDIN=1





//OLED底层设置函数
void OLED_WR_Byte(unsigned dat,unsigned cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(unsigned dat); 
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
void OLED_WriteDataBlock(u8 code *_data, u8 count);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 Char_size);
void OLED_ShowString(u8 x,u8 y, u8 *p,u8 Char_Size);
void OLED_ClearToEndLine(u8 x, u8 y, u8 Char_Size);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowChinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char code *BMP);
//void fill_picture(unsigned char fill_Data);
void Picture();

void IIC_Start();
void IIC_Stop();
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);
bit IIC_Wait_Ack();

#endif
