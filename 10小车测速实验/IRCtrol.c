#include "IRCtrol.h"
#include "interface.h"

unsigned char ir_rec_flag=0;//接收数据标志位 1 有新数据 0 没有
unsigned char IRCOM[4];

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
* 函 数 名 ：DelayIr
* 函数功能 ：0.14MS 延时
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void DelayIr(unsigned char x)  
{
 unsigned char i;
  while(x--)
 {
  for (i = 0; i<68; i++) {}
 }
}

/*******************************************************************************
* 函 数 名 ：Exit0Init
* 函数功能 ：外中断0初始化程序
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Exit0Init()
{
	EX0 = 1;	//使能 INT1 外部中断
    IT0 = 1;	// 触发方式为脉冲负边沿触发
    EA = 1;//总中断
}  

/*******************************************************************************
* 函 数 名 ：Exit0Int
* 函数功能 ：外部中断0 ISR
* 输    入 ：无
* 输    出 ：无
*******************************************************************************/
void Exit0Int() interrupt 0 
{
  unsigned char j,k,N=0;

     EX0 = 0;   
	 DelayIr(15);
	 if (IRIN==1) 
     { EX0 =1;
	   return;
	  } 
		 continue_time = 40;//连发信号，表示指令持续 40*5 = 200ms 无指令停车
                           //确认IR信号出现
  while (!IRIN)            //等IR变为高电平，跳过9ms的前导低电平信号。
    {DelayIr(1);}

 for (j=0;j<4;j++)         //收集四组数据
 { 
  for (k=0;k<8;k++)        //每组数据有8位
  {
   while (IRIN)            //等 IR 变为低电平，跳过4.5ms的前导高电平信号。
     {DelayIr(1);}
    while (!IRIN)          //等 IR 变为高电平
     {DelayIr(1);}
     while (IRIN)           //计算IR高电平时长
      {
    DelayIr(1);
    N++;           
    if (N>=30)
	 { EX0=1;
	 return;}                  //0.14ms计数过长自动离开。
      }                        //高电平计数完毕                
     IRCOM[j]=IRCOM[j] >> 1;                  //数据最高位补“0”
     if (N>=8) {IRCOM[j] = IRCOM[j] | 0x80;}  //数据最高位补“1”
     N=0;
  }//end for k
 }//end for j
   
   if (IRCOM[2]!=~IRCOM[3])
   { EX0=1;
     return; }
	 
		 //指令转换
		 
		 switch(IRCOM[2])
		 {
			 case 0x46: ctrl_comm = COMM_UP;break;
			 case 0x15: ctrl_comm = COMM_DOWN;break;
			 case 0x44: ctrl_comm = COMM_LEFT;break;
			 case 0x43: ctrl_comm = COMM_RIGHT;break;
			 case 0x40: ctrl_comm = COMM_STOP;break;
			 default : EX0 = 1; return;
		 }
		 ir_rec_flag = 1;

     EX0 = 1; 
} 

