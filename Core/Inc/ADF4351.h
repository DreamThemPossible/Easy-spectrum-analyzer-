#ifndef _ADF4351_H_
#define _ADF4351_H_
#include "main.h"

#define ADF4351_CE 					PBout(12)
#define ADF4351_LE 					PBout(13)
#define ADF4351_OUTPUT_DATA 		PBout(14)
#define ADF4351_CLK					PBout(15)

//#define ADF4351_INPUT_DATA PCin(10)

#define ADF4351_RF_OFF	((u32)0XEC801C)

void ADF4351Init(void);
//void ReadToADF4351(u8 count, u8 *buf);
void WriteToADF4351(u8 count, u8 *buf);
void WriteOneRegToADF4351(u32 Regster);
void ADF4351_Init_some(void);
void ADF4351WriteFreq(float Fre);		//	(xx.x) M Hz

#endif

