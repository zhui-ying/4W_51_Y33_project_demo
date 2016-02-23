#ifndef __LCD1602_H_
#define __LCD1602_H_

#include <reg52.h>
#include <intrins.h>

void LCD1602Init();
void LCD1602WriteCommand(char comm);
#endif
