#include "LCD1602.h"
#include "interface.h"

//全局变量定义
unsigned char code table1[]="Hantech MCU";
unsigned char code table2[]="Command:";

/*******************************************************************************
* 函 数 名 ：Delayms
* 函数功能 ：实现 ms级的延时
* 输    入 ：ms
* 输    出 ：无
*******************************************************************************/
static void Delayms(unsigned int ms)
{
	unsigned int i,j;
	for(i=0;i<ms;i++)
	#if FOSC == 11059200L
		for(j=0;j<114;j++);
	#elif FOSC == 12000000L
	  for(j=0;j<123;j++);
	#elif FOSC == 24000000L
		for(j=0;j<249;j++);
	#elif FOSC == 48000000L
		for(j=0;j<715;j++);
	#else
		for(j=0;j<114;j++);
	#endif
}

/*******************************************************************************
* 函 数 名 ：Delayus
* 函数功能 ：实现 us级的延时
* 输    入 ：us
* 输    出 ：无
*******************************************************************************/
void Delayus(unsigned int us)
{
	unsigned int i;
	for(i=0;i<us;i++)
	_nop_();
}

/*******************************************************************************
* 函 数 名 ：LcdBusy
* 函数功能 ：检查LCD忙状态,LcdBusy 为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据
* 输    入 ：无
* 输    出 ：状态
*******************************************************************************/
bit LcdBusy()
 {                          
    bit result;
    lcd_rs = 0;
    lcd_rw = 1;
    lcd_en = 1;
    Delayus(1);
    result = (bit)(P0&0x80);
    lcd_en = 0;
    return(result); 
}
 
/*******************************************************************************
* 函 数 名 ：write_com
* 函数功能 ：LCD1602 写指令
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void LcdWriteCom(unsigned char com)
{
	while(LcdBusy());
	LCD_PORT=com;
	lcd_rs=0;
	lcd_rw = 0;
	lcd_en=0;
	Delayus(5);
	lcd_en=1;
	Delayus(5);
	lcd_en=0;
	Delayus(5);
}

/*******************************************************************************
* 函 数 名 ：write_com
* 函数功能 ：LCD1602 写数据
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void LcdWriteDate(unsigned char date)
{
	while(LcdBusy());
	LCD_PORT=date;
	lcd_rs=1;
	lcd_rw = 0;
	lcd_en=0;
	Delayus(5);
	lcd_en=1;
	Delayus(5);
	lcd_en=0; 
	Delayus(5);	
}

/*******************************************************************************
* 函 数 名 ：LCD1602Init
* 函数功能 ：LCD1602初始化
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void LCD1602Init()
{
	char index=0;
	LcdWriteCom(0x38);  //设置16*2显示，8位数据接口
	LcdWriteCom(0x0c); //开显示，显示光标且闪烁
	LcdWriteCom(0x06);//写一个指针自动加一
	LcdWriteCom(0x01);//清屏  
	Delayms(100);//延时一段时间时间，等待LCD1602稳定	
	
	LcdWriteCom(0x80);//设置第一行 数据地址指针
	for(index=0;index<11;index++)
	{
		LcdWriteDate(table1[index]);  //写入数据             
	}
	
	LcdWriteCom(0xc0);//设置第二行 数据地址指针
	for(index=0;index<8;index++)
	{
		LcdWriteDate(table2[index]);  //写入数据             
	}
}

/*******************************************************************************
* 函 数 名 ：LCD1602WriteCommand
* 函数功能 ：显示指令到屏幕 U D L R S 
* 输    入 ：comm 字符格式
* 输    出 ：无
*******************************************************************************/
void LCD1602WriteCommand(char comm)
{
	LcdWriteCom(0xc0 + 9);
	LcdWriteDate(comm);  //写入数据   
}

