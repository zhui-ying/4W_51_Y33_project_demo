#ifndef __IRCTROL_H_
#define __IRCTROL_H_

#include <reg52.h>
#include <intrins.h>
#define FOSC 48000000L //晶振设置，使用24M Hz 6T双速倍模式

extern unsigned char ir_rec_flag;//接收数据标志位 1 有新数据 0 没有
extern char ctrl_comm;//控制指令
extern unsigned char continue_time;
void Exit0Init();

#endif
