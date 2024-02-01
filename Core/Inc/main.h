/* USER CODE BEGIN Header */
/**

  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
typedef uint32_t u32;
typedef uint8_t u8;
typedef uint16_t u16;

#define BITBAND(addr, bitnum)			((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)					*((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)			MEM_ADDR(BITBAND(addr, bitnum))

/**********************************************************
                       GPIO地址映射
              基地�?加上寄存器偏移地�?组成
**********************************************************/

#define GPIOA_ODR_Addr    (GPIOA_BASE+12)	//0x4001080C
#define GPIOB_ODR_Addr    (GPIOB_BASE+12)	//0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12)	//0x4001100C
#define GPIOD_ODR_Addr    (GPIOD_BASE+12)	//0x4001140C
#define GPIOE_ODR_Addr    (GPIOE_BASE+12)	//0x4001180C
#define GPIOF_ODR_Addr    (GPIOF_BASE+12)	//0x40011A0C
#define GPIOG_ODR_Addr    (GPIOG_BASE+12)	//0x40011E0C

#define GPIOA_IDR_Addr    (GPIOA_BASE+8)	//0x40010808
#define GPIOB_IDR_Addr    (GPIOB_BASE+8)	//0x40010C08
#define GPIOC_IDR_Addr    (GPIOC_BASE+8)	//0x40011008
#define GPIOD_IDR_Addr    (GPIOD_BASE+8)	//0x40011408
#define GPIOE_IDR_Addr    (GPIOE_BASE+8)	//0x40011808
#define GPIOF_IDR_Addr    (GPIOF_BASE+8)	//0x40011A08
#define GPIOG_IDR_Addr    (GPIOG_BASE+8)	//0x40011E08

/**********************************************************
             实现快�?�单�?IO操作，类似于51的IO操作
                   n值要小于IO具体数目
**********************************************************/

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
