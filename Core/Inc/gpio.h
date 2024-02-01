/* USER CODE BEGIN Header */
/**
  ******************************************************************************
   KEY configure
PA1	K1
PA2	K2
PA3	K3

PB0	K4
PB1	K5
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "delay.h"
#include "ADF4351.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define YES	  1
#define NO	  0

#define T_10mS	((u32)10)
#define T_1S	((u32)1000)
#define T_330mS	((u32)330)

#define K_1		((uint16_t)0b011110)
#define K_2		((uint16_t)0b011101)
#define K_3		((uint16_t)0b011011)
#define K_4		((uint16_t)0b010111)
#define K_5		((uint16_t)0b001111)

//////////////////////////////

////////码�?�重映射////////////
#define K_1_S		0X01        //K_M_S  -->> 按键_中键_短按
#define K_2_S		0X02
#define K_3_S		0X03
#define K_4_S		0X04
#define K_5_S		0X05

#define K_1_L		0X0B        //K_M_S  -->> 按键_中键_短按
#define K_2_L		0X0C
#define K_3_L		0X0D
#define K_4_L		0X0E
#define K_5_L		0X0F

#define K_NO		0X00000000
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
extern u32 KEY_Sys_Timer;
extern u32 KEY_Time;
extern u32 key_Trgtime;
extern u32 KEY_Trg;
extern u32 KEY_Cont;
extern u32 Trg_state;
extern u32 Keycode;

extern u32 count1;
extern u32 count1max;

u32 KeyRead(void);
void KEY_EXIT(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

