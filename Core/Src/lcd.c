/**********************************************************
其这里是OLED驱动；使用6脚OLED没有reset
PA4  DC
PA5  cs
PA6  CLK
PA7  SDA
**********************************************************/

#include "delay.h"
#include "lcd.h"
#include "stdlib.h"
#include "oledfont.h"
#include "chinese.h"
#include "string.h"
u8 LCD_GRAM[128][8];
const u8 LCD_GRAM_Zeros[128][8] = {0};
/*已在gpio.h初始化LCD_GPIO*/

/*写指令到 LCD 模块*/
void transfer_command(int data1)
{
	char i;

	//cs1=0;
	rs=0;

	for(i=0;i<8;i++)
	{
		sclk=0;
		if(data1&0x80) sid=1;
		else sid=0;
		sclk=1;
		data1<<=1;
	}
}

/*写数据到 LCD 模块*/
void transfer_data(int data1)
{
	char i;

	//cs1=0;
	rs=1;

	for(i=0;i<8;i++)
	{
		sclk=0;
		if(data1&0x80) sid=1;
		else sid=0;
		sclk=1;
		data1<<=1;
	}
}

/*LCD 模块初始化*/
void initial_lcd(void)
{

	//cs1=0;
	reset=0; /*低电平复位*/
	delay_ms(100);
	reset=1; /*复位完毕*/
	delay_ms(20);
	transfer_command(0xAE);//--turn off oled panel
	transfer_command(0x00);//---set low column address
	transfer_command(0x10);//---set high column address
	transfer_command(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	transfer_command(0x81);//--set contrast control register
	transfer_command(0xCF);// Set SEG Output Current Brightness
	transfer_command(0xA1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	transfer_command(0xC8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	transfer_command(0xA6);//--set normal display
	transfer_command(0xA8);//--set multiplex ratio(1 to 64)
	transfer_command(0x3f);//--1/64 duty
	transfer_command(0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	transfer_command(0x00);//-not offset
	transfer_command(0xd5);//--set display clock divide ratio/oscillator frequency
	transfer_command(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	transfer_command(0xD9);//--set pre-charge period
	transfer_command(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	transfer_command(0xDA);//--set com pins hardware configuration
	transfer_command(0x12);
	transfer_command(0xDB);//--set vcomh
	transfer_command(0x40);//Set VCOM Deselect Level
	transfer_command(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	transfer_command(0x02);//
	transfer_command(0x8D);//--set Charge Pump enable/disable
	transfer_command(0x14);//--set(0x10) disable
	transfer_command(0xA4);// Disable Entire Display On (0xa4/0xa5)
	transfer_command(0xA6);// Disable Inverse Display On (0xa6/a7)
	LCD_Clear();
	transfer_command(0xAF);



}

/*设置LCD地址*/
void lcd_address(u8 page,u8 column)
{
	//cs1=0;
	column=column;  //我们平常所说的第 1 列,在 LCD 驱动 IC 里是第 0 列.所以这里减去 1.
	page=page;
	//设置页地址.每页是第 8 行.一个画面的 64 行被分成 8 个页. 我们平常所说的第 1 页,在 LCD 驱动 IC 里是第 0 页.所以这里减去 1.*/
	transfer_command(0xb0+page);
	transfer_command(((column>>4)&0x0f)+0x10); //设置列地址的高 4 位
	transfer_command(column&0x0f);  //设置列地址的低 4 位
}

//更新显存到LCD
void LCD_Refresh_Gram(void)
{
	u8 i,n;
	//cs1=0;
	for(i=0;i<8;i++)
	{
		lcd_address(i,0);
		for(n=0;n<128;n++)
		{
			transfer_data(LCD_GRAM[n][i]);
		}
	}
	//cs1=1;
}
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void LCD_GRAM_Clear()
{
		memcpy(LCD_GRAM,LCD_GRAM_Zeros,sizeof(LCD_GRAM));
}
void LCD_Clear(void)
{

	memcpy(LCD_GRAM,LCD_GRAM_Zeros,sizeof(LCD_GRAM));
	LCD_Refresh_Gram();//更新显示
}
void LCD_draw_Square(u8 x1,u8 y1,u8 x2,u8 y2)
{
	LCD_Fill(x1,y1,x1,y2,1);
	LCD_Fill(x1,y1,x2,y1,1);
	LCD_Fill(x1,y2,x2,y2,1);
	LCD_Fill(x2,y1,x2,y2,1);
}

//画点
//x:0~127
//y:0~63
//t:1 填充 0,清空
void LCD_draw_Point(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	//cs1=0;
	pos=y/8;
	bx=y%8;
	temp=1<<bx;
	if(t)LCD_GRAM[x][pos]|=temp;
	else LCD_GRAM[x][pos]&=~temp;
	//cs1=1;
}
void LCD_Move_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127)	x -= 127;
	if(y>63)	y -= 63;
	pos=y/8;
	bx=y%8;
	temp=1<<(bx);
	if(t)LCD_GRAM[x][pos]|=temp;
	else LCD_GRAM[x][pos]&=~temp;
}
void LCD_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)
{
	u8 x,y;
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)LCD_draw_Point(x,y,dot);
	}
//	LCD_Refresh_Gram();//更新显示
}

//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
//dot:0,清空;1,填充

void Set_Point(u8 x,u8 y)
{
	static u8 time=0,set=5,xbuf=0,ybuf=0;

	time++;
	if(xbuf!=x || ybuf!=y)	LCD_Fill(xbuf,ybuf,xbuf,ybuf+16,0);
	if(time < set)
	{
		LCD_Fill(x,y,x,y+16,1);
	}else if(time < set*2)
	{
		LCD_Fill(x,y,x,y+16,0);
	}else time=0;
	xbuf=x;
	ybuf=y;
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示
//size:选择字体 16/12
void LCD_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//得到偏移后的值
    for(t=0;t<size;t++)
    {
			if(size==12)temp=asc2_1206[chr][t];  //调用1206字体
			else temp=asc2_1608[chr][t];		 //调用1608字体
			for(t1=0;t1<8;t1++)
			{
				if(temp&0x80)LCD_draw_Point(x,y,mode);
				else LCD_draw_Point(x,y,!mode);
				temp<<=1;
				y++;
				if((y-y0)==size)
				{
					y=y0;
					x++;
					break;
				}
			}
    }
}
void LCD_Show_chineseChar(u8 x,u8 y,char *chr, u8 mode)
{
	u8 temp,t,t1;
	u8 y0=y;

	for(t=0;t<32;t++)
	{
		temp = *chr;
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
				LCD_Move_DrawPoint(x,y,mode);
			else
			LCD_Move_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
		}
		chr++;
	}
}
void	WriteA_Chinese(u8 mode,unsigned char x, unsigned char y, const u8 *s)
{
	unsigned char j;

	if (x > 127) return;
	if (y > 63) return;

	for (j = 0; j < sizeof(GB_16)/34;j++)
	{
		if(s[0] == (char)GB_16[j].Index[0] && s[1] == (char)GB_16[j].Index[1])
		{
			LCD_Show_chineseChar(x, y, &GB_16[j].Msk[0], mode);
			x+=16;
			break;
		}
	}
}
//显示字符串
//x,y:起点坐标
//*p:字符串起始地址
//用16字体
void LCD_ShowString(u8 mode,u8 x,u8 y,const u8 *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58
    while(*p!='\0')
    {
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;LCD_Clear();}
        LCD_ShowChar(x,y,*p,16,mode);
        x+=8;
        p++;
    }
}
void LCD_ShowString_12(u8 mode,u8 x,u8 y,const u8 *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58
    while(*p!='\0')
    {
        if(x>MAX_CHAR_POSX){x=0;y+=16;}
        if(y>MAX_CHAR_POSY){y=x=0;LCD_Clear();}
        LCD_ShowChar(x,y,*p,12,mode);
        x+=8;
        p++;
    }
}
void LCD_ShowAllString(u8 mode,u8 x,u8 y,const u8 *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58
    while(*p!='\0')
    {
			if(x>MAX_CHAR_POSX)return;
			if(y>MAX_CHAR_POSY)return;
			if(*p>127)
			{
				WriteA_Chinese(mode,x,y,p);
				p+=2;
				x+=16;
			}else
			{
				LCD_ShowChar(x,y,*p,16,mode);
				x+=8;
				p++;
			}
    }
}

void	Write_Chinese_String(u8 mode,unsigned char x, unsigned char y, char *s)
{
	unsigned char i, j;

	if (x > 127) return;
	if (y > 63) return;
	for (i = 0; s[i] != '\0'; i+=2)
	{
		for (j = 0; j < sizeof(GB_16)/34;j++)
		{
			if(s[i] == (char)GB_16[j].Index[0] && s[i + 1] == (char)GB_16[j].Index[1])
			{
				LCD_Show_chineseChar(x, y, &GB_16[j].Msk[0], mode);
				x+=16;
				break;
			}
		}
	}
}
void LCD_Show_CEStr(u16 x0,u16 y0, u8 *pp)
{
	LCD_ShowAllString(1,x0,y0*8,pp);
}
void LCD_Show_ModeCEStr(u16 x0,u16 y0, u8 *pp, u8 show_mode)
{
	LCD_ShowAllString(show_mode,x0,y0*8,pp);

}

void OLED_ShowString(u8 x,u8 y,u8 *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{
	if(chr[j]>=128)
	{
		chr[j]-=128;
		LCD_ShowChar(x,y*8,chr[j],16,0);
	}else
	LCD_ShowChar(x,y*8,chr[j],16,1);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}



/***********************************************************
* 函数名称：LCD_DrawLine
* 函数功能：LCD画线函数
*           入口参数: x1,y1（起点坐标）x2,y2（终点坐标）c(颜色)
*			出口参数：无
***********************************************************/

// 交换整数 a 、b 的值
void swap_int(u16 *a, u16 *b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

// Bresenham's line algorithm

void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u8 c)
{
    u16 dx =abs( x2 - x1), // 水平距离
        dy =abs( y2 - y1), // 垂直距离
        yy = 0;    		   // 角度是否大于45°标志

	int ix ,
        iy,
        cx ,
        cy ,
        n2dy,
        n2dydx ,
        d ;
    if(dx < dy)  // 当斜率大于 1 时，dx、dy 互换, 即角度大于45°
    {
        yy = 1;              // 设置标志位
        swap_int(&x1, &y1);  // 交换坐标
        swap_int(&x2, &y2);
        swap_int(&dx, &dy);
    }
    ix = (x2 - x1) > 0 ? 1 : -1; // 判断是否加1还是减一
    iy = (y2 - y1) > 0 ? 1 : -1;
    cx = x1;  //  x初值
    cy = y1;  //  y初值
    n2dy = dy * 2;    		 // 求要比较的各个差值
    n2dydx = (dy - dx) * 2;
    d = dy * 2 - dx;

// 如果直线与 x 轴的夹角大于45度
    if(yy)
    {
        while(cx != x2) // 计数是否达到终点
        {
            if(d < 0)
            {
                d += n2dy;
            }
            else
            {
                cy += iy;
                d += n2dydx;
            }
            LCD_draw_Point( cy, cx, c); // 画点
            cx += ix;
        }
    }

// 如果直线与 x 轴的夹角小于45度
    else
    {
        while(cx != x2)
        {
            if(d < 0)
            {
                d += n2dy;
            }
            else
            {
                cy += iy;
                d += n2dydx;
            }
            LCD_draw_Point( cx, cy, c);
            cx += ix;
        }
    }
}

/**********************************************************
* 函数名称：LCD_DrawRectangle
* 函数功能：LCD画矩形函数
*           入口参数: x1,y1（对角线起点坐标）x2,y2（对角线终点坐标）color(颜色)
*			出口参数：无
***********************************************************/

void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u8 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);
	LCD_DrawLine(x1,y1,x1,y2,color);
	LCD_DrawLine(x1,y2,x2,y2,color);
	LCD_DrawLine(x2,y1,x2,y2,color);
}

/**********************************************************
* 函数名称：LCD_Drawcircle
* 函数功能：在指定位置画一个指定大小的圆
*           入口参数: (x,y)-(中心点坐标）r(半径)
*                     fill(为是否填充)   c(颜色)
*			出口参数：无
***********************************************************/

void draw_circle_8( u16 xc, u16 yc, u16 x, u16 y, u8 c)
	{
	    // 参数 c 为颜色值
	    LCD_draw_Point( xc + x, yc + y, c);
	    LCD_draw_Point( xc - x, yc + y, c);
	    LCD_draw_Point( xc + x, yc - y, c);
	    LCD_draw_Point( xc - x, yc - y, c);
	    LCD_draw_Point( xc + y, yc + x, c);
	    LCD_draw_Point( xc - y, yc + x, c);
	    LCD_draw_Point( xc + y, yc - x, c);
	    LCD_draw_Point( xc - y, yc - x, c);
	}
//Bresenham's circle algorithm
void LCD_Drawcircle( u16 xc, u16 yc, u16 r, u16 fill, u8 c)
	{
	    // (xc, yc) 为圆心，r 为半径
	    // fill 为是否填充
	    // c 为颜色值
	    // 如果圆在图片可见区域外，直接退出

	   int x = 0, y = r, yi, d;
	       d = 3 - 2 * r;

//	    if(xc + r < 0 || xc - r >= 128 || yc + r < 0 || yc - r >= 64)
//	    {
//	        return;
//	    }
	    if(fill)
	    {
	        // 如果填充（画实心圆）
	        while(x <= y)
	        {
	            for(yi = x; yi <= y; yi ++)
	            {
	                draw_circle_8(xc, yc, x, yi, c);
	            }
	            if(d < 0)
	            {
	                d = d + 4 * x + 6;
	            }
	            else
	            {
	                d = d + 4 * (x - y);
	                y --;
	            }

	            x++;
	        }
	    }
	    else
	    {
	        // 如果不填充（画空心圆）
	        while (x <= y)
	        {
	            draw_circle_8(xc, yc, x, y, c);
	            if(d < 0)
	            {
	                d = d + 4 * x + 6;
	            }
	            else
	            {
	                d = d + 4 * (x - y);
	                y --;
	            }
	            x ++;
	        }
	    }
	}





