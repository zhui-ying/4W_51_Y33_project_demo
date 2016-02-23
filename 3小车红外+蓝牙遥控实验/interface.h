#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include <reg52.h>

//IO 引脚定义
//LCD1602 显示屏
#define LCD_PORT P2
sbit lcd_en=P0^5;
sbit lcd_rs=P0^7;
sbit lcd_rw=P0^6;

//串口蓝牙
//sbit Rx = P3^0;
//sbit Tx = P3^1;

//红外遥控
sbit IRIN = P3^2;         //红外接收器数据线,外部中断0

//超声波模块
sbit Echo = P3^3;//返回IO口 外部中断1
sbit Trig = P3^4;//触发IO口

//电机驱动
sbit FRONT_LEFT_F_IO =  P1^0; //左前前进IO
sbit FRONT_LEFT_B_IO =  P1^1; //左前后退IO

sbit FRONT_RIGHT_F_IO =  P1^7; //右前前进IO
sbit FRONT_RIGHT_B_IO =  P1^6; //右前后退IO

sbit BEHIND_LEFT_F_IO =  P1^3; //左后前进IO
sbit BEHIND_LEFT_B_IO =  P1^2; //左后后退IO

sbit BEHIND_RIGHT_F_IO =  P1^4; //右后前进IO
sbit BEHIND_RIGHT_B_IO =  P1^5; //右后后退IO

//红外循迹
sbit SEARCH_M_IO = P0^3;//中间循迹IO
sbit SEARCH_R_IO = P0^2;//右循迹IO
sbit SEARCH_L_IO = P0^1;//左循迹IO
#define BLACK_AREA 1
#define WHITE_AREA 0

//红外壁障
sbit VOID_R_IO = P0^4;//右边
sbit VOID_L_IO = P0^0;//左边
#define BARRIER_Y 0 //有障碍物
#define BARRIER_N 1  //无障碍物

//舵机IO
sbit DUOJI_IO = P3^5;//开发板默认是P0^7,这里需要转到其他IO口上

//速度码盘反馈
sbit FRONT_RIGHT_S_IO =  P3^6; //右前前进IO
sbit FRONT_LEFT_S_IO =  P3^7; //左前前进IO

//系统定义
#define FOSC 48000000L //晶振设置，使用24M Hz 6T双速倍模式
#define BAUD 9600 //波特率
#define TIME_US 100 //0.1ms 周期定时时间

//电机运动控制

//左前
#define FRONT_LEFT_GO    FRONT_LEFT_F_IO = 1; FRONT_LEFT_B_IO = 0//前进
#define FRONT_LEFT_BACK  FRONT_LEFT_F_IO = 0; FRONT_LEFT_B_IO = 1//后退
#define FRONT_LEFT_STOP  FRONT_LEFT_F_IO = 0; FRONT_LEFT_B_IO = 0//停止

//右前
#define FRONT_RIGHT_GO     FRONT_RIGHT_F_IO = 1;FRONT_RIGHT_B_IO = 0
#define FRONT_RIGHT_BACK   FRONT_RIGHT_F_IO = 0;FRONT_RIGHT_B_IO = 1
#define FRONT_RIGHT_STOP   FRONT_RIGHT_F_IO = 0;FRONT_RIGHT_B_IO = 0

//左后
#define BEHIND_LEFT_GO     BEHIND_LEFT_F_IO = 1;BEHIND_LEFT_B_IO = 0
#define BEHIND_LEFT_BACK   BEHIND_LEFT_F_IO = 0;BEHIND_LEFT_B_IO = 1
#define BEHIND_LEFT_STOP   BEHIND_LEFT_F_IO = 0;BEHIND_LEFT_B_IO = 0

//右后
#define BEHIND_RIGHT_GO    BEHIND_RIGHT_F_IO = 1;BEHIND_RIGHT_B_IO = 0
#define BEHIND_RIGHT_BACK  BEHIND_RIGHT_F_IO = 0;BEHIND_RIGHT_B_IO = 1
#define BEHIND_RIGHT_STOP  BEHIND_RIGHT_F_IO = 0;BEHIND_RIGHT_B_IO = 0

#define SPEED_DUTY 30//默认占空比 按1ms最小分辨率 周期50ms计算

//指令定义
#define COMM_STOP  'I'//停止
#define COMM_UP    'A'//前进
#define COMM_DOWN  'B'//后退
#define COMM_RIGHT 'D'//右转
#define COMM_LEFT  'C'//左转

#endif
