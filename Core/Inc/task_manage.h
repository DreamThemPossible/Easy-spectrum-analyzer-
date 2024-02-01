#ifndef __task_manage_H
#define __task_manage_H
#include "main.h"
#include "lcd.h"
#include "gpio.h"

#define Interface 3//界面总数
#define StrMax    10//缓存数据大小
#define CIRCLE 	5

extern u32 SysTimer;
extern u16 spectrum_buf[1000];



u8 Task_Delay(u32 delay_time, u8* delay_ID);
u8 TaskCycleDelay(u32 delay_time, u8* Last_delay_ID, u8* Self_delay_ID);
void welcome_KW(void);
void Main_Menu(u32 Key_Value, u8* Task_ID);
void LCD_Show_ModeCEInfo(u16 x0, u8 start_info, u8 current_deal_info);
void Copybuf2dis(u8 *source, u8 dis[StrMax], u8 dispoint, u8 FloatNum, u8 CursorEn);
void Set_PointFre(u32 Key_Value, u8* Task_ID);
void Sweep_Fre(u32 Key_Value, u8* Task_ID);
void Move_Pha(u32 Key_Value, u8* Task_ID);
void Jump_Fre(u32 Key_Value, u8* Task_ID);

//
void Task0_PointFre(u32 Key_Value);
void Task1_SweepFre(u32 Key_Value);
void Task2_DrawSpectrum(u32 Key_Value);

void fre_buf_change(u8 *strbuf);

#endif
