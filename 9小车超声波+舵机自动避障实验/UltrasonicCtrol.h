#ifndef __ULTRASONICCTROL_H_
#define __ULTRASONICCTROL_H_

#include <reg52.h>
#include <intrins.h>

extern volatile unsigned int distance_cm;//µ±«∞æ‡¿Î
void UltraSoundInit();
void GetDistance();
void StartGetDistance();
void Distance();
void GetDistanceDelay();

#endif
