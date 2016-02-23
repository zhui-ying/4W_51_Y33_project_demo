/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：小车超声波+红外跟踪实验
* 实验说明 ：使用超声波判断前方引导物的距离，使用红外来判断引导物的移动方向
* 实验平台 ：流星7号、51单片机最小系统
* 连接方式 ：请参考interface.h文件
* 注    意 ：1.因地面和轮子的差异，左右转向时需要根据实际需要调节差速的差值
*            2.需要耐心调节红外避障的感应距离以及超声波控制距离
*            3.建议使用不反光的书本作为引导物
* 作    者 ：航太电子产品研发部    QQ ：1909197536
* 店    铺 ：http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "LCD1602.h"
#include "interface.h"
#include "motor.h"
#include "UltrasonicCtrol.h"

//全局变量定义
unsigned int speed_count=0;//占空比计数器 50次一周期
char front_left_speed_duty=SPEED_DUTY;
char front_right_speed_duty=SPEED_DUTY;
char behind_left_speed_duty=SPEED_DUTY;
char behind_right_speed_duty=SPEED_DUTY;

unsigned char tick_5ms = 0;//5ms计数器，作为主函数的基本周期
unsigned char tick_1ms = 0;//1ms计数器，作为电机的基本计数器
unsigned char tick_200ms = 0;//刷新显示

char ctrl_comm;//控制指令
char ctrl_comm_last = COMM_STOP;//上一次的指令
//unsigned char continue_time = 0;

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


//障碍物查询和处理,一旦出现障碍物，禁止直行
void BarrierProc()
{
	
		if(VOID_L_IO == BARRIER_N && VOID_R_IO == BARRIER_Y)//右转
	{
		CarRight();
		Delayms(30);
		return;
	}
	else if(VOID_L_IO == BARRIER_Y && VOID_R_IO == BARRIER_N)//左转
	{
		CarLeft();
		Delayms(30);
		return;
	}	
	
	if((distance_cm >= 5 && distance_cm <= 7) || distance_cm > 50) //
	{
		CarStop();//防震荡
		return;
	}
	
	if(VOID_L_IO == VOID_R_IO)
	{
		if(distance_cm < 5) CarBack();
		if(distance_cm > 7) CarGo();	
	}
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
	LCD1602Init();
	Timer0Init();
	UltraSoundInit();
	//5ms 执行一次
	while(1)
	{ 
		if(tick_5ms >= 5)
		{
			tick_5ms = 0;
			tick_200ms++;
			if(tick_200ms >= 40)
			{
				tick_200ms = 0;
				LCD1602WriteDistance(distance_cm);
			}
			
			//do something
			Distance();
			BarrierProc();
//			if(ctrl_comm_last != ctrl_comm)//指令发生变化
//			{
//				ctrl_comm_last = ctrl_comm;
//				switch(ctrl_comm)
//				{
//					case COMM_UP:    CarGo();break;
//					case COMM_DOWN:  CarBack();break;
//					case COMM_LEFT:  CarLeft();break;
//					case COMM_RIGHT: CarRight();break;
//					case COMM_STOP:  CarStop();break;
//					default : break;
//				}
//				LCD1602WriteCommand(ctrl_comm);
//			}
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

