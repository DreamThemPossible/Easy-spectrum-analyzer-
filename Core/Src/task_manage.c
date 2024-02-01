/**********************************************************
                       康威电子

功能：stm32f103c8t6控制，25MHz时钟， AD9833正弦波点频输出，范围0-10M(可编辑0-100M)，
			方波：0-500K，三角波：0-1M，扫频默认正弦波 。
			显示：12864cog
接口：控制接口请参照AD9833.h  按键接口请参照key.h

**********************************************************/

#include "task_manage.h"
#include "delay.h"
#include "gpio.h"
#include "ADF4351.h"
#include "tim.h"
#include "adc.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
//#define OUT_KEY  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)//读取按键0
#define FLASH_SAVE_ADDR  0x0801F000  				//设置FLASH 保存地址(必须为偶数)


u8 Task_Index = 0;//界面切换
u8 Param_Mode = 0;//参数类型切换
u8 fre_buf[StrMax]={'\0'}; //参数转换字符缓存
u8 display[StrMax]={'\0'}; //点频页面显示缓存
u16 spectrum_buf[1000] = {0};
u8 P_Index = 0; //编辑位置
u8 Task_First=1;//第一次进入标记
u8 _return=0;
//扫频参数
u32 SweepMinFre  =  60000;//khz
u32 SweepMaxFre  = 100000;
u32 SweepStepFre =    100;
u16 SweepTime    =     3 ;//ms
u32 CenterFre_Ave=  80000;//khz
u32 CenterFre[CIRCLE]= {80000};
u8 SweepFlag = 0;

u8 circle = 0;

u8 Task_Delay(u32 delay_time, u8* delay_ID)
{
	static u8 Time_Get = 0;
	static u32 Time_Triger;
    if(Time_Get == 0)
    {
      Time_Triger = SysTimer + delay_time;
      Time_Get = 1;
    }
    if(SysTimer >= Time_Triger)
    {
      Time_Get = 0;
			*delay_ID = 1;		//	后续代码已被执行一遍
			return 1;
    }
		return 0;
}


u8 TaskCycleDelay(u32 delay_time, u8* Last_delay_ID, u8* Self_delay_ID)
{
	static u8 Time_Get = 0;
	static u32 Time_Triger;

	if(!(*Last_delay_ID))
		return 0;
	if(Time_Get == 0)
	{
		Time_Triger = SysTimer + delay_time;
		Time_Get = 1;
	}
	if(SysTimer >= Time_Triger)
	{
		Time_Get = 0;
		*Last_delay_ID = 0;
		*Self_delay_ID = 1;		//	后续代码已被执行一遍
		return 1;
	}
	return 0;
}

//把数据放到另一个缓存，显示，FloatNum小数点位数，CursorEn光标使能
void Copybuf2dis(u8 *source, u8 dis[StrMax], u8 dispoint, u8 FloatNum, u8 CursorEn)
{
	int i, len;

	len = strlen(source);
	i = len - FloatNum;//整数个数
	if(FloatNum>0)dis[i] = '.';
	for (i = 0; i < len; i++)
	{
		if(i < (len-FloatNum)) dis[i] = source[i];
		else
		{ dis[i+1] = source[i]; }
	}

	if(CursorEn)
	{
		if(dispoint < (len-FloatNum)) dis[dispoint] += 128;
		else dis[dispoint+1] += 128;
	}
}
//
void Set_PointFre(u32 Key_Value, u8* Task_ID)
{
	//按键判断
	if(Task_Index != 2){
	switch(Key_Value)
	{
		case K_4_S: fre_buf[P_Index]++;break;
		case K_4_L: fre_buf[P_Index]++;break;
		case K_5_L: P_Index++;break;
		case K_5_S: P_Index++;break;
		case K_1_L: P_Index--;break;
		case K_1_S: P_Index--;break;
		case K_3_S: fre_buf[P_Index]--;break;
		case K_3_L: fre_buf[P_Index]--;break;
		case K_2_S: Param_Mode++;break;
	}}
	if(fre_buf[P_Index] == '/') fre_buf[P_Index] = '9';//<'0'
	if(fre_buf[P_Index] == ':') fre_buf[P_Index] = '0';//>'9'
	//界面切换
	if(Key_Value == K_2_L)
	{
		Task_Index++;
		LCD_Clear();
	}
	if(Key_Value == K_2_L || Key_Value == K_2_S) Task_First=1;//更新第一次进入
	if(Task_Index >= Interface) Task_Index=0;
	switch(Task_Index)
	{
		case 0:
			Task0_PointFre(Key_Value);
			break;
		case 1:
			Task1_SweepFre(Key_Value);
			break;
		case 2:
			Task2_DrawSpectrum(Key_Value);
	}
	//其他处理
	if(Task_Index == 1 || Task_Index == 2)
	{
		SweepFlag = 1;
		ADC_Value = HAL_ADC_GetValue(&hadc1);
		if(ADC_1stepVmax < ADC_Value) ADC_1stepVmax = ADC_Value;
		if(ADC_1stepVmin > ADC_Value) ADC_1stepVmin = ADC_Value;
	}
	else
		SweepFlag = 0;

}
//任务
//
void Task0_PointFre(u32 Key_Value)
{
	static u32 SinFre = 80000;//第一次进入页面默认输出
	u8 showstr[StrMax]={0};

	if(Task_First)
	{
		Task_First = 0;
		Key_Value = K_2_S;
		sprintf(fre_buf, "%7d", SinFre);//第一次 进入
		LCD_Show_CEStr(64-8*3,0,"DotFrq");

		_return=1;
	}
	if(Key_Value != K_NO)
	{
		//判断
		if(P_Index == 255)
			P_Index = 6;
		P_Index = P_Index%7;//数据位数
		SinFre = atol(fre_buf);//字符转换数字，判断上下限
//		SinFre =
//		((u32)fre_buf[0]-'0')*1000000+
//		((u32)fre_buf[1]-'0')*100000+
//		((u32)fre_buf[2]-'0')*10000+
//		((u32)fre_buf[3]-'0')*1000+
//		((u32)fre_buf[4]-'0')*100+
//		((u32)fre_buf[5]-'0')*10+
//		((u32)fre_buf[6]-'0');
		if(SinFre>4400000) SinFre=4400000;//数据限制
		if(SinFre<45000) SinFre=45000;
		sprintf(fre_buf, "%7d", SinFre);//字符转换
		//显示
		sprintf(showstr, "%7d", SinFre);//字符转换
		fre_buf_change(showstr);//fre_buf当中 ‘ '->'0'
		Copybuf2dis(showstr, display, P_Index, 0, 1);
		OLED_ShowString(64-4*11, 3, display);
		LCD_Show_CEStr(64-4*11+7*8,3,"kHz");
		//数据处理写入
		ADF4351WriteFreq((double)SinFre/1000);

		_return=1;
	}
}


void Task1_SweepFre(u32 Key_Value)//扫频
{
	u8 showstr[StrMax]={0};

	if(Task_First)
	{
		Task_First = 0;//清除第一次进入标记
		Key_Value = K_2_S;
		Param_Mode %= 4;//计算参数模式，以便装入初始值
		if(Param_Mode == 0) sprintf(fre_buf, "%7d", SweepMinFre);
		if(Param_Mode == 1) sprintf(fre_buf, "%7d", SweepMaxFre);
		if(Param_Mode == 2) sprintf(fre_buf, "%4d", SweepStepFre);
		if(Param_Mode == 3) sprintf(fre_buf, "%3d", SweepTime);


		LCD_Show_CEStr(64-8*3,0,"SwpFrq");//模式名称
		_return=1;//更新显示标志
	}
	if(Key_Value != K_NO)
	{

		if(Param_Mode == 0)//最小频率设置
		{
			P_Index %= 7;//参数位数
			SweepMinFre = atol(fre_buf);//字符转换
			if(SweepMinFre>SweepMaxFre) SweepMinFre=SweepMaxFre;//数据限制
			if(SweepMinFre<45000) SweepMinFre=45000;
			sprintf(fre_buf, "%7d", SweepMinFre);//数据重新写入
		}
		//显示
		sprintf(showstr, "%7d", SweepMinFre);//重新申请缓存显示
		fre_buf_change(showstr);//fre_buf当中 ‘ '->'0'
		if(Param_Mode == 0) Copybuf2dis(showstr, display, P_Index, 0, 1);
		else Copybuf2dis(showstr, display, P_Index, 0, 0);
		OLED_ShowString(64-4*11+16, 2, display);
		LCD_Show_CEStr(64-4*11+7*8+16,2,"kHz");
		LCD_Show_CEStr(0,2,"Min:");

		if(Param_Mode == 1)//最大频率设置
		{
			P_Index %= 7;//参数位数
			SweepMaxFre = atol(fre_buf);//字符转换
			if(SweepMaxFre>4400000) SweepMaxFre=4400000;//数据限制
			if(SweepMaxFre<50000) SweepMaxFre=50000;
			sprintf(fre_buf, "%7d", SweepMaxFre);//数据重新写入
		}
		//显示
		sprintf(showstr, "%7d", SweepMaxFre);//重新申请缓存显示
		fre_buf_change(showstr);//fre_buf当中 ‘ '->'0'
		if(Param_Mode == 1) Copybuf2dis(showstr, display, P_Index, 0, 1);
		else Copybuf2dis(showstr, display, P_Index, 0, 0);
		OLED_ShowString(64-4*11+16, 4, display);
		LCD_Show_CEStr(64-4*11+7*8+16,4,"kHz");
		LCD_Show_CEStr(0,4,"Max:");

		if(Param_Mode == 2)//频率步进设置
		{
			P_Index %= 5;//参数位数
			SweepStepFre = atol(fre_buf);//字符转换
			if(SweepStepFre>99999) SweepStepFre=99999;//数据限制
			if(SweepStepFre<0) SweepStepFre=0;
			sprintf(fre_buf, "%5d", SweepStepFre);//数据重新写入
		}
		//显示
		sprintf(showstr, "%5d", SweepStepFre);//重新申请缓存显示
		fre_buf_change(showstr);//fre_buf当中 ‘ '->'0'
		if(Param_Mode == 2) Copybuf2dis(showstr, display, P_Index, 0, 1);
		else Copybuf2dis(showstr, display, P_Index, 0, 0);
		display[5]=0;//只显示4位
		OLED_ShowString(0, 6, display);
		LCD_Show_CEStr(8*5,6,"kHz");

		if(Param_Mode == 3)//步进时间设置
		{
			P_Index %= 3;//参数位数
			SweepTime = atol(fre_buf);//字符转换
			if(SweepTime>999) SweepTime=999;//数据限制
			if(SweepTime<1) SweepTime=1;
			sprintf(fre_buf, "%3d", SweepTime);//数据重新写入
		}
		//显示
		sprintf(showstr, "%3d", SweepTime);//重新申请缓存显示
		fre_buf_change(showstr);//fre_buf当中 ‘ '->'0'
		if(Param_Mode == 3) Copybuf2dis(showstr, display, P_Index, 0, 1);
		else Copybuf2dis(showstr, display, P_Index, 0, 0);
		display[3]=0;//只显示3位
		OLED_ShowString(72, 6, display);
		LCD_Show_CEStr(72+3*8,6,"ms");
		//数据处理写入

		_return=1;
	}
}
void Task2_DrawSpectrum(u32 Key_Value)
{
	int jj;
//	for(jj=0;jj<CIRCLE;jj++)//排除开始那个奇怪的中心频率
//		if(CenterFre[jj]-SweepMinFre < 2000) CenterFre[jj] = CenterFre[(jj+1)%CIRCLE];
//	u16 spectrum_showbuf[400];
	if(count1 == count1max)
	{

		// draw spectrum mode,to find center frequency
		for(jj=0,ADC_1stepVmax=0; jj < count1max; jj++)//从FreMin->FreMax找中心频率对应的幅度最大值
			if(spectrum_buf[jj] > ADC_1stepVmax)//ADC_1stepVmax作为临时变量求最大值用
			{
				ADC_1stepVmax = spectrum_buf[jj];
				CenterFre[circle] = SweepMinFre+SweepStepFre*jj;
			}
		ADC_1circleVmax = ADC_1stepVmax;//用来绘图幅度归一化
		ADC_1stepVmax = 0;
		circle++;//已完成轮数，用来求多轮中心频率平均值

		if(circle >= CIRCLE)
		{
			circle=0;
			LCD_GRAM_Clear();
			LCD_draw_Square(12,20,112,60);//绘制方框
			LCD_Show_CEStr(0, 0, display);
			LCD_Show_CEStr(6*8, 0, "kHz60-100M");


		for(jj = 0;jj < 100;jj++)//绘制+-10M的频谱x:12-111 y:60-20

			LCD_draw_Point(12+jj,(int)(100 - (float)spectrum_buf[4*jj]/ADC_1circleVmax *80),1);

		_return=1;//更新显示标志
		}

	}

	CenterFre_Ave = 0;//求中心频率
	for(jj=0; jj<CIRCLE; jj++) CenterFre_Ave += CenterFre[jj];
	CenterFre_Ave =(uint32_t)(CenterFre_Ave / CIRCLE);

	sprintf(display, "%6d", CenterFre_Ave);
	fre_buf_change(display);//fre_buf当中 ‘ '->'0'
	display[6] = '\0';//只显示6位




}
//
void fre_buf_change(u8 *strbuf)
{
	int i;
	for (i = 0 ; i < strlen(strbuf); i++)
	 if(strbuf[i]==0x20) strbuf[i] = '0';
	for (i = 0 ; i < strlen(fre_buf); i++)
	 if(fre_buf[i]==0x20) fre_buf[i] = '0';
}

