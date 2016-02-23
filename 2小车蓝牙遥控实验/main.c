/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：小车蓝牙遥控实验
* 实验说明 ：使用手机连接蓝牙模块后，通过手机发送指令来控制小车
* 实验平台 ：流星7号、51单片机最小系统
* 连接方式 ：请参考interface.h文件
* 注    意 ：指令必须连续发送才会使小车动作，若停止发送指令，则小车会停止
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "LCD1602.h"
#include "interface.h"
#include "motor.h"

//全局变量定义
unsigned int speed_count=0;//占空比计数器 50次一周期
char front_left_speed_duty=SPEED_DUTY;
char front_right_speed_duty=SPEED_DUTY;
char behind_left_speed_duty=SPEED_DUTY;
char behind_right_speed_duty=SPEED_DUTY;

unsigned char tick_5ms = 0;//5ms计数器，作为主函数的基本周期
unsigned char tick_1ms = 0;//1ms计数器，作为电机的基本计数器

char ctrl_comm;//控制指令
unsigned char continue_time = 0;
unsigned char bt_rec_flag=0;//蓝牙控制标志位

/*******************************************************************************
* 函 数 名 ：Delayms
* 函数功能 ：实现 ms级的延时
* 输    入 ：ms
* 输    出 ：无
*******************************************************************************/
void Delayms(unsigned int ms)
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

char putchar(char ch)
{ 
	/* Place your implementation of fputc here */ 
	SBUF=(unsigned char)ch; //将接收到的数据放入到发送寄存器
	while(!TI);		  //等待发送数据完成
	TI=0;		 //清除发送完成标志位	
	return ch;
}

/*******************************************************************************
* 函 数 名 ：UsartConfiguration
* 函数功能 ：串口设置
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void UsartConfiguration()
{
	SCON = 0X50;			//设置为工作方式1	10位异步收发器
	TMOD |= 0x20; //设置计数器工作方式2  8位自动重装计数器	
	PCON = 0X80;//波特率加倍	SMOD = 1  19200
	TH1 = 256 -(FOSC/12/32/(BAUD/2)); //计算溢出率
	TL1 = 256 -(FOSC/12/32/(BAUD/2));
	TR1 = 1; //打开定时器	
	ES=1;//打开串口
	EA = 1;//打开总中断
}

/*******************************************************************************
* 函 数 名 ：Timer0Init
* 函数功能 ：定时器0初始化
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Timer0Init()
{
	TMOD|=0x01; //设置定时器0工作方式为1
	TH0=(65536-(FOSC/12*TIME_US)/1000000)/256;
	TL0=(65536-(FOSC/12*TIME_US)/1000000)%256;
	ET0=1; //开启定时器0中断
	TR0=1;	//开启定时器	
	EA=1;  //打开总中断
}


/*******************************************************************************
* 函 数 名 ：main
* 函数功能 ：主函数
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void main()
{
	CarStop();	
	UsartConfiguration();
	LCD1602Init();
	Timer0Init();
	
	//5ms 执行一次
	while(1)
	{ 
		if(tick_5ms >= 5)
		{
			tick_5ms = 0;
			continue_time--;//200ms 无接收指令就停车
			if(continue_time == 0)
			{
				continue_time = 1;
				CarStop();
			}
			//do something
			if(bt_rec_flag == 1)//接收到红外信号
			{
				bt_rec_flag = 0;
				switch(ctrl_comm)
				{
					case COMM_UP:    CarGo();break;
					case COMM_DOWN:  CarBack();break;
					case COMM_LEFT:  CarLeft();break;
					case COMM_RIGHT: CarRight();break;
					case COMM_STOP:  CarStop();break;
					default : break;
				}
				LCD1602WriteCommand(ctrl_comm);
			}
		}
	}
}

/*******************************************************************************
* 函 数 名 ：Timer0Int
* 函数功能 ：定时器0中断函数 ， 每隔TIME_MS ms进入
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Timer0Int() interrupt 1
{
	TH0=(65536-(FOSC/12*TIME_US)/1000000)/256;
	TL0=(65536-(FOSC/12*TIME_US)/1000000)%256;
	tick_1ms++;
	if(tick_1ms >= 10)
	{
		tick_1ms = 0;
		speed_count++;
		tick_5ms++;
		if(speed_count >= 50)
		{
			speed_count = 0;
		}
		CarMove();
	}
}

/*******************************************************************************
* 函 数 名 ：UsartInt
* 函数功能 ：串口中断服务函数
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void UsartInt() interrupt 4
{
	unsigned char rec_data;
	if(RI == 1)
	{
		rec_data=SBUF; 
		bt_rec_flag = 1;
		ctrl_comm = rec_data;
		RI = 0;           //清除接收中断标志位
		continue_time = 40;
	}	 
}

