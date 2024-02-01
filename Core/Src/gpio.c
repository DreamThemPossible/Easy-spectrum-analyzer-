/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "adc.h"
#include "task_manage.h"

extern u8 Task_Index;
extern u32 CenterFre[CIRCLE];
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA1 PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA5 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
//..................................key..............................
//////////////////////
u32 KEY_Sys_Timer;//10ms中断计数
u32 Keycode;
//////////////////////
u32 SystemTime;//1ms中断计数
u32 KEY_Time;//按下时长(ms)
u32 key_Trgtime;//10ms消抖
u32 KEY_Trg;
u32 KEY_Cont=0;
u32 Trg_state = NO;

u32 Change_GPIOCode(void)//refresh keycode scan
{
	u32 GPIO_Data = 0;
	GPIO_Data += HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) << 4 ;	//K5 =>PB1
	GPIO_Data += HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0) << 3;	//K4 =>PB0

	GPIO_Data += HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3) << 2;	//K3 =>PA3
	GPIO_Data += HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2) << 1;	//K2 =>PA2
	GPIO_Data += HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1);		//K1 =>PA1
	return GPIO_Data;// 0x0000 00_000X XXXX
}
u32 KeyRead(void)
{
    u32 ReadData = Change_GPIOCode();
    KEY_Trg = ReadData & (ReadData ^ KEY_Cont);    //KEY_Cont代表上一时刻的ReadData
	//KEY_Trg体现按键变化(按下或松�??)
    KEY_Cont = ReadData;

	if(!Trg_state)
		key_Trgtime = KEY_Sys_Timer + T_10mS;//按下的时�??+10ms，用来确认松�???消抖

	if(KEY_Trg)//按键松开
		Trg_state = YES;

	if(Trg_state)
	{
		if((KEY_Sys_Timer > key_Trgtime)&&(KEY_Sys_Timer<(((u32)0-T_10mS))))  //消抖 && 确保中断计数未溢
		{
			switch(KEY_Cont)
			{
				case K_1:{Keycode = K_1_S; break;}
				case K_2:{Keycode = K_2_S; break;}
				case K_3:{Keycode = K_3_S; break;}
				case K_4:{Keycode = K_4_S; break;}
				case K_5:{Keycode = K_5_S; break;}
				default:{Keycode = K_NO; break;}
			}

		}
	}


	if(KEY_Time >= T_1S)	  	//
	{
		KEY_Time = T_1S-T_330mS;
		switch(KEY_Cont)
		{
			case K_1:{Keycode = K_1_L; break;}
			case K_2:{Keycode = K_2_L; break;}
			case K_3:{Keycode = K_3_L; break;}
			case K_4:{Keycode = K_4_L; break;}
			case K_5:{Keycode = K_5_L; break;}
			default:{Keycode = K_NO; break;}
		}
	}
	return Keycode;
}
void KEY_EXIT(void)
{
	if(Keycode != K_NO)
	{
		Trg_state = NO;
		Keycode = K_NO;
	}
}

/*------------------------按键时长计数器TIM3中断-1ms---------------------------*/
//-----------------------TIM4-扫频控制
u16 sysbufer = 0;
u32 SysTimer = 0;



u32 count = 0, count1 = 0, NowFre = 0, count1max = 0;
extern u32 SweepMinFre;
extern u32 SweepMaxFre;
extern u32 SweepStepFre;
extern u16 SweepTime;//ms
extern u8 SweepFlag;//1-》sweeping frequency; 0-》not sweeping

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{



	if (htim->Instance == TIM3)//1ms中断
	{
		KEY_Sys_Timer++;//1ms中断次数
		if(KEY_Cont!=31)//0b011111
		  KEY_Time++;		//按下时长
		else
		  KEY_Time = 0;


		sysbufer++;
		if(sysbufer == 10)
		{
		  SysTimer++;       //10ms中断次数
		  sysbufer = 0;
		}
	}

	if (htim->Instance == TIM4)//x0.1ms =>1ms
	{
		if(SweepFlag)
		{
			count++;


			if(count>=10*SweepTime)//频率步进，切换频点时
			{
				count=0;
				if(ADC_1stepVmax >= ADC_1stepVmin) spectrum_buf[count1] = ADC_1stepVmax-ADC_1stepVmin;
				else spectrum_buf[count1] = 0;
				ADC_1stepVmax = 0;
				ADC_1stepVmin = 0;

				count1++;//扫频进度

				NowFre = SweepMinFre+SweepStepFre*count1;

				if(NowFre>SweepMaxFre)//一轮扫频结束
				{

					count1max = count1-1;
					count1 = 0;

				}
				//refresh frequency

				ADF4351WriteFreq((double)NowFre/1000);

			}
		}

	}
}


/* USER CODE END 2 */
