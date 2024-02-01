
#include "delay.h"

//=========================================================
//#if _USER_SysTick	//定义了则使用SysTick定时器做延时函数计数
//=========================================================

/**********************************************************
                     定义计算变量
**********************************************************/

static uint8_t  fac_us=0;	//us延时倍乘数
static uint16_t fac_ms=0;	//ms延时倍乘数

/**********************************************************
* 函数功能 ---> 初始化延时函数
* 入口参数 ---> SYSCLK：系统工作最高的频率。单位M
* 返回数值 ---> none
* 功能说明 ---> 主要是初始化SysTick寄存器
**********************************************************/
void delay_init(uint8_t SYSCLK)
{
	SysTick->CTRL &= 0xfffffffb;	//bit2清空，选择外部时钟HCLK/8，停止计数
	fac_us = SYSCLK/8;	//系统时钟的1/8
	fac_ms = (uint16_t)fac_us*1000;	//ms需要的SysTick时钟数
}
/**********************************************************
* 函数功能 ---> 延时n个us
* 入口参数 ---> nus：要延时的us数
* 返回数值 ---> none
* 功能说明 ---> none
**********************************************************/		    								   
void delay_us(uint32_t nus)
{		
	uint32_t temp;
		    	 
	SysTick->LOAD = nus*fac_us; //时间加载
	SysTick->VAL = 0x00;        //清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      //开始倒数
	do
	{
		temp = SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));	//等待时间到达
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL = 0x00;	//清空计数器
}
/**********************************************************
* 函数功能 ---> 延时n个ms
* 入口参数 ---> nus：要延时的us数
* 返回数值 ---> none
* 功能说明 ---> SysTick->LOAD为24位寄存器,所以,最大延时为:
*               nms <= 0xffffff*8*1000/SYSCLK
*               SYSCLK单位为Hz,nms单位为ms
*               注意nms的范围 0 ~ 1864(72M情况下)
**********************************************************/ 
void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;
			   
	SysTick->LOAD = (uint32_t)nms*fac_ms;	//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL = 0x00;           //清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      //开始倒数
	do
	{
		temp = SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));	//等待时间到达
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL = 0x00;	//清空计数器
}


