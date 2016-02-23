#include "UltrasonicCtrol.h"
#include "interface.h"

volatile unsigned char status = 0;//程序当前状态，0,空闲 1 发送触发信号，2 等待信号返回,3 
unsigned int dis_count = 0;//脉宽长计时
volatile unsigned int distance_cm = 0;//当前距离
unsigned char t2_full_count = 0;//计数器计满次数计数
static unsigned int tick_5ms = 0;//5ms计数器

void UltraSoundInit()
{
	Trig = 0;
	TH2 = RCAP2H = 0;
	TL2 = RCAP2L = 0;  
	TR2 = 0;//关闭定时器	
	ET2=1;             //允许T0中断
	
	EX1=0;        //关闭外部中断1
	IT1=1;        //由高电平变低电平，触发外部中断
	PX1 = 1;//优先级较高
}

//中断的方式读取距离值 每5ms执行一次
void GetDistance()
{
		if(status == 1)
		{
			//发送触发信号
			Trig = 1;
			status = 1;
			TH2 = 0;
			TL2 = 0;
			TR2 = 1;//打开定时器
			while(TL2 < 42);//延时超过10us
			status = 2;
			Trig = 0;
			TH2 = 0;
			TL2 = 0;
			TR2 = 0;
			while(Echo == 0);//等待回向信号起始位置
			EX1 = 1;//打开外部中断
			TR2 = 1;
			status = 3;//开始计算距离
	    }
		if(status == 4)//成功接收到数据
		{
			distance_cm = (unsigned int)(((long)(dis_count) * 34)/8000);//声速340m/s
			status = 0;//准备下次发送
		}
		if(status == 5)//接收数据失败
		{
			status = 0;//准备下次发送
		}
}

//开始获取距离，每100ms执行一次
void StartGetDistance()
{
	status = 1;
}

void Distance()
{
	tick_5ms++;
	if(tick_5ms >= 15)
	{
		tick_5ms = 0;
//		StartGetDistance();
		GetDistanceDelay();
	}
	//GetDistance();
}

//延时的方式读取距离值
void GetDistanceDelay()
{
		//发送触发信号
		Trig = 1;
		status = 1;
		TH2 = 0;
		TL2 = 0;
		TR2 = 1;//打开定时器
		while(TL2 < 42);//延时超过10us
		status = 2;
		Trig = 0;
		TH2 = 0;
		TL2 = 0;
		TR2 = 0;
		while(Echo == 0);//等待回向信号起始位置
//			EX1 = 1;//打开外部中断
		TR2 = 1;
		while(Echo == 1)//开始计算长度
		{
			if(status == 5) 
			{
				status = 0;
				distance_cm = 0;//失败后就后退
				EA = 1;		//开中断
				return;//本次失败
			}
		}
		dis_count = TH2*256 + TL2;
		TR2 = 0;
		distance_cm = (unsigned int)(((long)(dis_count) * 34)/8000);//声速340m/s
		status = 0;//准备下次发送		
}

/*******************************************************************************
* 函 数 名 ：
* 函数功能 ：外部中断1服务函数
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Exint1Int()  interrupt 2   // 外部中断是2号
 { 
		if(status == 3)
		{
			 dis_count = TH2*256 + TL2;
			 EX1=0;            //关闭外部中断
			 TR2 = 0;
			 status = 4;
		}
  }
 
/*******************************************************************************
* 函 数 名 ：Timer0Int
* 函数功能 ：定时器0中断服务函数
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Timer2Int() interrupt 5 // 定时器0中断是1号
{
	TF2 = 0;
	EX1=0; 
	status = 5;//超时
}

