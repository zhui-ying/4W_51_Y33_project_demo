#include "SpeedCtrol.h"
#include "interface.h"

//轮子直径66mm，光电码盘齿数为20，轮子周长 207mm = 20.7cm 
//程序采用判断高低电平变化次数计数，也就是说轮子转一周计数次数为40
//一个计数变化表示轮子跑过的距离为 20.7/40 = 0.5175cm

unsigned char front_left_speed=0;
unsigned char front_right_speed=0;

unsigned char front_left_speed_temp=0;
unsigned char front_right_speed_temp=0;

static bit front_left_io=0;
static bit front_right_io=0;
static unsigned char count_5ms=0;

/*******************************************************************************
* 函 数 名 ：MeasureSpeed
* 函数功能 ：速度测量，计算IO变化次数，该函数必须每5ms调用一次
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void MeasureSpeed()
{
	count_5ms++;
	if(FRONT_LEFT_S_IO != front_left_io)//发生电平变化
	{
		front_left_speed_temp++;
		front_left_io = FRONT_LEFT_S_IO;
	}
	
	if(FRONT_RIGHT_S_IO != front_right_io)//发生电平变化
	{
		front_right_speed_temp++;
		front_right_io = FRONT_RIGHT_S_IO;
	}
	
	if(count_5ms == 100)//每500ms获取一次速度
	{
		count_5ms = 0;
		front_left_speed = front_left_speed_temp *2;//获取1s的高低电平变化次数
		front_right_speed = front_right_speed_temp*2;
		front_left_speed_temp = 0;
		front_right_speed_temp = 0;
		
		front_left_speed = (unsigned char)(0.5175 * (double)front_left_speed + 0.5);//计算速度 cm/s 四舍五入
		front_right_speed = (unsigned char)(0.5175 * (double)front_right_speed + 0.5);//计算速度 cm/s 四舍五入
	}
}

/*******************************************************************************
* 函 数 名 ：ClearMeasure
* 函数功能 ：清除测量，重新开始测量
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void ClearMeasure()
{
	count_5ms = 0;
	front_left_speed_temp = 0;
	front_right_speed_temp = 0;
}


