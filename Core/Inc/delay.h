
#ifndef __DELAY_H__
#define __DELAY_H__


#include "main.h"

#define _USER_SysTick					1	/* 定义是否使用SysTick定时器做延时计数函数 */
											//0：使用其他方式
											//1：使用SysTick定时器做延时函数计数
//#if _USER_SysTick	//定义了则使用SysTick定时器做延时函数计数

void delay_init(uint8_t SYSCLK);	//初始化延时函数
void delay_us(uint32_t nus);	//延时n个us
void delay_ms(uint16_t nms);	//延时n个ms




#endif//__DELAY_H__

