#include "config.h"
#include "oledfont.h"

// ��OLED_Init�����У�������OLED_Clear�����ĵ���������Դ��е��������

// 批量写数据字节到 OLED (SSD1306列地址自动递增)
// 用函数替代宏避免 C51 非重入函数变量覆盖问题
void OLED_WriteDataBlock(u8 code *_data, u8 count)
{
    u8 idx;
    IIC_Start();
    Write_IIC_Byte(IIC_SLAVE_ADDR); IIC_Wait_Ack();
    Write_IIC_Byte(0x40);           IIC_Wait_Ack();
    for(idx = 0; idx < count; idx++) {
        Write_IIC_Byte(_data[idx]);  IIC_Wait_Ack();
    }
    IIC_Stop();
}

//OLED���Դ棬��Ÿ�ʽ����:
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127


//===================================================================
// IIC 时序延迟 (针对 STC15W4K32S4 @ 11.0592MHz 1T 模式)
// 1 NOP ≈ 90.4ns
// SCL 已改为推挽输出(快速边沿), SDA 为准双向(无外部上拉)
// 使用 40 NOPs 得到约 3.6μs 半周期延时
// SCL ≈ 110-130kHz, 给 SDA 弱上拉足够建立时间
//===================================================================
#define IIC_DELAY_CNT    40
#define IIC_DELAY()  do {                                               \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
} while(0)

// ACK 采样前等待 (SDA 准双向上升较慢)
#define IIC_DELAY_ACK() do {                                            \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
    _nop_();_nop_();_nop_();_nop_();_nop_();                           \
} while(0)

/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{
    OLED_SCLK_Set() ;
    OLED_SDIN_Set();
    IIC_DELAY();
    OLED_SDIN_Clr();
    IIC_DELAY();
    OLED_SCLK_Clr();
    IIC_DELAY();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
    OLED_SDIN_Clr();
    IIC_DELAY();
    OLED_SCLK_Set() ;
    IIC_DELAY();
    OLED_SDIN_Set();
    IIC_DELAY();
}

// ACK 等待: 释放 SDA 线, 产生第9个 SCL 时钟
// 返回: 0=ACK(正常), 1=NACK(异常)
bit IIC_Wait_Ack(void)
{
    bit ack;
    OLED_SDIN_Set();            // 释放 SDA (准双向口: 写1=关断下拉, 靠上拉维持)
    IIC_DELAY_ACK();            // 等 SDA 升到逻辑高
    OLED_SCLK_Set();            // 第9个时钟脉冲
    IIC_DELAY_ACK();            // 等从机拉低 SDA
    ack = OLED_SDIN;            // 采样 SDA: 0=ACK, 1=NACK
    OLED_SCLK_Clr();
    IIC_DELAY();
    return ack;
}

/**********************************************
// IIC Write byte
**********************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
    unsigned char i;
    for(i=0; i<8; i++)
    {
        OLED_SCLK_Clr();
        if(IIC_Byte & 0x80) OLED_SDIN_Set();
        else OLED_SDIN_Clr();
        IIC_DELAY();      // �ȴ� SDA �ȶ�
        OLED_SCLK_Set();
        IIC_DELAY();      // SCL �ߵ�ƽ����
        IIC_Byte <<= 1;
    }
    OLED_SCLK_Clr();
    IIC_DELAY();
}

/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
    IIC_Start();
    Write_IIC_Byte(IIC_SLAVE_ADDR);            //Slave address,SA0=0
    IIC_Wait_Ack();	
    Write_IIC_Byte(0x00);			//write command
    IIC_Wait_Ack();	
    Write_IIC_Byte(IIC_Command); 
    IIC_Wait_Ack();	
    IIC_Stop();
}

/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
    IIC_Start();
    Write_IIC_Byte(IIC_SLAVE_ADDR);			//D/C#=0; R/W#=0
    IIC_Wait_Ack();	
    Write_IIC_Byte(0x40);			//write data
    IIC_Wait_Ack();	
    Write_IIC_Byte(IIC_Data);
    IIC_Wait_Ack();	
    IIC_Stop();
}

void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
    if(cmd)
    {
        Write_IIC_Data(dat);
    }
    else
    {
        Write_IIC_Command(dat);
    }
}

/********************************************
// fill_Picture
********************************************/
/*
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);		//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_WR_Byte(fill_Data,1);
			}
	}
}
*/

/***********************Delay****************************************/
/*
void Delay_50ms(unsigned int Del_50ms)
{
	unsigned int m;
	for(;Del_50ms>0;Del_50ms--)
		for(m=6245;m>0;m--);
}

void Delay_1ms(unsigned int Del_1ms)
{
	unsigned char j;
	while(Del_1ms--)
	{	
		for(j=0;j<123;j++);
	}
}
*/

//��������
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{
    IIC_Start();
    Write_IIC_Byte(IIC_SLAVE_ADDR); // 0x78
    IIC_Wait_Ack();
    Write_IIC_Byte(0x00);           // ���������͵������� (Co=0, D/C#=0)
    IIC_Wait_Ack();
    
    Write_IIC_Byte(0xb0 + y);                 // ����ҳ��ַ
    IIC_Wait_Ack();
    Write_IIC_Byte(((x & 0xf0) >> 4) | 0x10); // �����е�ַ��4λ
    IIC_Wait_Ack();
    Write_IIC_Byte(x & 0x0f);                 // �����е�ַ��4λ
    IIC_Wait_Ack();
    
    IIC_Stop();
}

//����OLED��ʾ   
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 

// //��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
void OLED_Clear(unsigned dat)
{
    u8 i, n;
    // 重置显示关键寄存器（防止I2C偶发干扰导致显示偏移）
    OLED_WR_Byte(0x2E, OLED_CMD);   // Deactivate scroll
    OLED_WR_Byte(0x40, OLED_CMD);   // Reset display start line = 0
    for(i = 0; i < 8; i++)
    {
        OLED_Set_Pos(0, i);
        IIC_Start();
        Write_IIC_Byte(IIC_SLAVE_ADDR);
        IIC_Wait_Ack();
        Write_IIC_Byte(0x40); // ����д����ģʽ
        IIC_Wait_Ack();
        for(n = 0; n < 128; n++)
        {
            Write_IIC_Byte(dat);
            IIC_Wait_Ack();
        }
        IIC_Stop();
    }
}

/*
void OLED_On(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} //������ʾ
}
*/

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ
//Char_Size:ѡ������ 16/12 
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size)
{
    unsigned char c = 0;
    uint16_t offset;
    c = chr - ' ';

    if (Char_Size == 16)
    {
        offset = (uint16_t)c << 4; // c * 16

        // 上半部分: 8 bytes 在单次 I2C 事务中连续写入
        OLED_Set_Pos(x, y);
        OLED_WriteDataBlock(&F8X16[offset], 8);

        // 下半部分: 8 bytes
        OLED_Set_Pos(x, y + 1);
        OLED_WriteDataBlock(&F8X16[offset + 8], 8);
    }
    else
    {
        // 12px字体: 6 bytes
        OLED_Set_Pos(x, y);
        OLED_WriteDataBlock(F6x8[c], 6);
    }
}


//m^n����
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)result*=m;
	return result;
}

//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//Char_size:�����С
//mode:ģʽ---0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 Char_size)
{
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(Char_size/2)*t,y,' ',Char_size);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(Char_size/2)*t,y,temp+'0',Char_size); 
	}
}



//显示一个字符串
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size)
{
    unsigned char j = 0;
    while(chr[j] != '\0')
    {
        OLED_ShowChar(x, y, chr[j], Char_Size);
        x += (Char_Size == 16) ? 8 : 6;
        if(x > 120) { x = 0; y += 2; }
        j++;
    }
}

// 清除从 (x, y) 到行尾的像素 (用于局部刷新时消除残留)
// y 是页地址(0-7), Char_Size=16时清除2页, =12时清除1页
void OLED_ClearToEndLine(u8 x, u8 y, u8 Char_Size)
{
    uint8_t remaining = 128 - x;
    uint8_t page_count = (Char_Size == 16) ? 2 : 1;
    uint8_t p, k;

    if (x >= 128) return;
    
    for (p = 0; p < page_count; p++) {
        OLED_Set_Pos(x, y + p);
        IIC_Start();
        Write_IIC_Byte(IIC_SLAVE_ADDR); IIC_Wait_Ack();
        Write_IIC_Byte(0x40);           IIC_Wait_Ack();
        for (k = 0; k < remaining; k++) {
            Write_IIC_Byte(0);          IIC_Wait_Ack();
        }
        IIC_Stop();
    }
}

//��ʾ����
void OLED_ShowChinese(u8 x,u8 y,u8 no)
{
    if (no >= max_chinese){
        return;
    }

    // 上半部分: 16 bytes 连续写入
    OLED_Set_Pos(x, y);
    OLED_WriteDataBlock(Hzk[2 * no], 16);

    // 下半部分: 16 bytes 连续写入
    OLED_Set_Pos(x, y + 1);
    OLED_WriteDataBlock(Hzk[2 * no + 1], 16);
}


/***********������������ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
// void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
// {
//  unsigned int j=0;
//  unsigned char x,y;
  
//   if(y1%8==0) y=y1/8;      
//   else y=y1/8+1;
// 	for(y=y0;y<y1;y++)
// 	{
// 		OLED_Set_Pos(x0,y);
//     for(x=x0;x<x1;x++)
// 	    {      
// 	    	OLED_WR_Byte(BMP[j++],OLED_DATA);
// 	    }
// 	}
// } 

void OLED_DrawBMP(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char code *BMP)
{
    unsigned char i;
    unsigned int k = 0;

    // ����ͼƬռ�õ�ҳ�� (ÿҳ8�����ظ߶�)
    unsigned char page_cnt = height / 8;
    if (height % 8 != 0)
    {
        page_cnt++;
    }

    // ��ҳд������
    for (i = 0; i < page_cnt; i++)
    {
        unsigned char j;
        OLED_Set_Pos(x, y + i);
        // 批量写入一行数据 (width bytes)
        IIC_Start();
        Write_IIC_Byte(IIC_SLAVE_ADDR); IIC_Wait_Ack();
        Write_IIC_Byte(0x40);           IIC_Wait_Ack();
        for (j = 0; j < width; j++) {
            Write_IIC_Byte(BMP[k++]);   IIC_Wait_Ack();
        }
        IIC_Stop();
    }
}



//初始化SSD1306 (无RES引脚, 用I2C总线复位替代硬件复位)
void OLED_Init(void)
{
    u8 i;

    // ---- I2C 总线复位序列: 产生9个SCL脉冲, 强迫卡住的从机复位 ----
    // 9个SCL脉冲后总线停在 SCL=低, SDA=高, 后续 IIC_Start() 会自动产生有效 START
    for (i = 0; i < 9; i++) {
        OLED_SDIN_Set();        // SDA = 高 (释放总线)
        IIC_DELAY();
        OLED_SCLK_Set();        // SCL 脉冲
        IIC_DELAY();
        OLED_SCLK_Clr();
        IIC_DELAY();
    }
    // ★ 注意: 这里不能手动 SDA下降(此时SCL=低, 不是START条件)
    // 复位后总线状态: SCL=低, SDA=高 → 空闲, 等第一个 IIC_Start() 做正确时序

    delay_ms(100);                          // 延长等待 (无RES脚, 完全靠内部POR)

    OLED_WR_Byte(0xAE,OLED_CMD);            //--display off
    OLED_WR_Byte(0x2E,OLED_CMD);            //--deactivate scroll (保险)

    OLED_WR_Byte(0x00,OLED_CMD);            //---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);            //---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);            //--set start line address
    OLED_WR_Byte(0xB0,OLED_CMD);            //--set page address
    OLED_WR_Byte(0x81,OLED_CMD);            // contract control
    OLED_WR_Byte(0xBF,OLED_CMD);            //--191 (not max 255, 降低对比度减轻残留)
    OLED_WR_Byte(0xA1,OLED_CMD);            //set segment remap
    OLED_WR_Byte(0xA6,OLED_CMD);            //--normal / reverse
    OLED_WR_Byte(0xA8,OLED_CMD);            //--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3F,OLED_CMD);            //--1/32 duty
    OLED_WR_Byte(0xC8,OLED_CMD);            //Com scan direction
    OLED_WR_Byte(0xD3,OLED_CMD);            //-set display offset
    OLED_WR_Byte(0x00,OLED_CMD);            //

    OLED_WR_Byte(0xD5,OLED_CMD);            //set osc division
    OLED_WR_Byte(0x80,OLED_CMD);            //

    OLED_WR_Byte(0xD8,OLED_CMD);            //set area color mode off
    OLED_WR_Byte(0x05,OLED_CMD);            //

    OLED_WR_Byte(0xD9,OLED_CMD);            //Set Pre-Charge Period
    OLED_WR_Byte(0xF1,OLED_CMD);            //Phase1:1 DCLK, Phase2:15 DCLK

    OLED_WR_Byte(0xDA,OLED_CMD);            //set com pin configuartion
    OLED_WR_Byte(0x12,OLED_CMD);            //

    OLED_WR_Byte(0xDB,OLED_CMD);            //set Vcomh
    OLED_WR_Byte(0x30,OLED_CMD);            //

    OLED_WR_Byte(0x8D,OLED_CMD);            //set charge pump enable
    OLED_WR_Byte(0x14,OLED_CMD);            //
    delay_ms(100);                          // 等待充电泵稳定 (DCDC startup time)

    OLED_WR_Byte(0xA4,OLED_CMD);            //--entire display ON from GDDRAM (确保不从RAM外显示)
    OLED_WR_Byte(0xAF,OLED_CMD);            //--turn on oled panel
}
