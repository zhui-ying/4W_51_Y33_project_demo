/********************************* 深圳市航太电子有限公司 *******************************
* 实 验 名 ：超声波舵机自动避障实验
* 实验说明 ：将超声波模块放在舵机旋转轴上，通过转动舵机，来获取前方，左边以及右边的障碍物距离
* 实验平台 ：流星7号、51单片机最小系统
* 连接方式 ：请参考interface.h文件
* 注    意 ：舵机旋转角度建议不要使用0°和180°，可以稍微往中间一点，比如选取20°和160 °，这样舵机旋转更精确
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

char ctrl_comm;//控制指令
char ctrl_comm_last = COMM_STOP;//上一次的指令
//unsigned char continue_time = 0;

unsigned char duoji_count=0;
unsigned char zhuanjiao = 11;

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


void DuojiMid()
{
	zhuanjiao = 11;
	Delayms(150);//延时1s
}

void DuojiRight()
{
	zhuanjiao = 6;
	Delayms(150);//延时1s
}

void DuojiLeft()
{
	zhuanjiao = 18;
	Delayms(150);//延时1s
}

///获取三个方向的距离,进来前舵机方向为向前
void GetAllDistance(unsigned int *dis_left,unsigned int *dis_right,unsigned int *dis_direct)
{
	CarStop();
	GetDistanceDelay();
	*dis_direct = distance_cm;
	
	DuojiRight();
	Delayms(100);
	GetDistanceDelay();//获取右边距离
	*dis_right = distance_cm;
	
	DuojiMid();
	DuojiLeft();
	Delayms(100);
	GetDistanceDelay();//获取左边距离
	*dis_left = distance_cm;
	
	DuojiMid();//归位
}

//障碍物查询和处理,一旦出现障碍物，禁止直行
void BarrierProcV1()
{
	if(distance_cm < 20)//前方有障碍物
	{
		unsigned int dis_left;//左边距离
		unsigned int dis_right;//右边距离
		unsigned int dis_direct;//右边距离
		if(distance_cm < 14)
		{
			CarBack();
			Delayms(500);
		}
		
		GetAllDistance(&dis_left,&dis_right,&dis_direct);
		
		if((dis_left < 14) || (dis_right < 14))
		{
			CarBack();
			Delayms(500);
			GetAllDistance(&dis_left,&dis_right,&dis_direct);
		}
		
		if(dis_left <= dis_right)
		{
			CarRight();
			Delayms(400);
		}else
		{
			CarLeft();
			Delayms(400);
		}
		
		GetAllDistance(&dis_left,&dis_right,&dis_direct);
		if(distance_cm < 14)
		{
			CarBack();
			Delayms(500);
			GetAllDistance(&dis_left,&dis_right,&dis_direct);
		}
		
		if(dis_direct >= dis_left && dis_direct >= dis_right)//前面最远就往前跑
		{
				CarGo();
		   GetDistanceDelay();
		   Delayms(100);
			return;
		}
		
		if((dis_left < 14) || (dis_right < 14))
		{
			CarBack();
			Delayms(500);
			GetAllDistance(&dis_left,&dis_right,&dis_direct);
		}
		
		if(dis_left <= dis_right)
		{
			CarRight();
			Delayms(400);
		}else
		{
			CarLeft();
			Delayms(400);
		}
		//CarGo();
		GetDistanceDelay();		
		
	}else
	{
		CarGo();
	}
}


void BarrierProc()
{
//	if(VOID_R_IO == BARRIER_Y)
//	{
//			CarBack();
//			Delayms(300);
//	}
//	if(VOID_L_IO == BARRIER_Y)
//	{
//			CarBack();
//			Delayms(300);
//	}
		if(distance_cm < 10)//前方有障碍物
	{
		unsigned int dis_left;//左边距离
		unsigned int dis_right;//右边距离
		unsigned int dis_direct;//右边距离
		if(distance_cm < 8)
		{
			CarBack();
			Delayms(400);
		}
		
		while(1)
		{
			GetAllDistance(&dis_left,&dis_right,&dis_direct);
			if(dis_direct < 5)
			{
				CarBack();
				Delayms(300);
				continue;
			}
			else if((dis_left < 5) || (dis_right < 5))
			{
				CarBack();
				Delayms(300);
				continue;
			}
			else if(dis_direct >= dis_left && dis_direct >= dis_right)//前方距离最远
			{
				CarGo();
				Delayms(600);
				return;
			}
			else if(dis_left <= dis_right)//右转
			{
				CarRight();
				Delayms(500);
			}
			else if(dis_right < dis_left)
			{
				CarLeft();
				Delayms(500);
			}
		}
	}
	else
	{
		CarGo();
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
//	LCD1602Init();
	Timer0Init();
//	while(1)
//	{
//		DuojiMid();
//		Delayms(1000);
//		DuojiLeft();
//		Delayms(1000);
//		DuojiMid();
//		Delayms(1000);
//		DuojiRight();
//		Delayms(1000);
//		
//	}
	distance_cm = 15;
	UltraSoundInit();
	DuojiMid();
	Delayms(1000);
	
	//5ms 执行一次
	while(1)
	{ 
		if(tick_5ms >= 5)
		{
			tick_5ms = 0;

			//do something
			Distance();
			BarrierProc();
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
	
		duoji_count++;
	if(duoji_count <= zhuanjiao)
	{
		DUOJI_IO = 0; //后面加了一个驱动，这边需要反相 modfied by LC 2015.09.19 22:39
	}
	else
	{
		DUOJI_IO = 1;
	}
	if(duoji_count >= 200)//20ms
	{
		duoji_count = 0;
	}
	
}

