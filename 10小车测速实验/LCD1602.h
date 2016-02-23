#ifndef __LCD1602_H_
#define __LCD1602_H_

#include <reg52.h>
#include <intrins.h>
#include "interface.h"

#define FOSC 48000000L //晶振设置，使用24M Hz 6T双速倍模式


void LCD1602Init();
void LCD1602WriteCommand(char comm);
void LCD1602WriteSpeed(unsigned char fl,unsigned char fr);
#endif
