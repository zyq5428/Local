#define GPICON                (*((volatile unsigned long *)0x7F008100))
#define GPJCON                (*((volatile unsigned long *)0x7F008120))
#define GPFCON                (*((volatile unsigned long *)0x7F0080A0))
#define MIFPCON               (*((volatile unsigned long *)0x7410800C))
#define SPCON                 (*((volatile unsigned long *)0x7F0081A0))
#define VIDCON0               (*((volatile unsigned long *)0x77100000))
#define VIDCON1               (*((volatile unsigned long *)0x77100004))
#define VIDTCON0              (*((volatile unsigned long *)0x77100010))
#define VIDTCON1              (*((volatile unsigned long *)0x77100014))
#define VIDTCON2              (*((volatile unsigned long *)0x77100018))
#define WINCON0               (*((volatile unsigned long *)0x77100020))
#define VIDOSD0A              (*((volatile unsigned long *)0x77100040))
#define VIDOSD0B              (*((volatile unsigned long *)0x77100044))
#define VIDOSD0C              (*((volatile unsigned long *)0x77100048))
#define VIDW00ADD0B0          (*((volatile unsigned long *)0x771000A0))
#define VIDW00ADD1B0          (*((volatile unsigned long *)0x771000D0))
#define VIDW00ADD2            (*((volatile unsigned long *)0x77100100))
#define DITHMODE              (*((volatile unsigned long *)0x77100170))
#define WIN0MAP               (*((volatile unsigned long *)0x77100180))
#define WPALCON               (*((volatile unsigned long *)0x771001A0))


#define VSPW 9 
#define VBPD 1
#define LINEVAL 271
#define VFPD 1
#define HSPW 40
#define HBPD 1
#define HOZVAL 479
#define HFPD 1
 

#define LeftTopX_F_Win0 0
#define LeftTopY_F_Win0 0

#define RightBotX_F_Win0 479
#define RightBotY_F_Win0 271

#define OSDSIZE_Win0 ((LINEVAL + 1) * (HOZVAL + 1))

#define OFFSIZE_F_Win0 0
#define PAGEWIDTH_F_Win0 480


unsigned long LCD_FRAME_BUFF[272][480];	//define frame buff

void lcd_gpio_init(void)
{
	//LCD VD[0-15]
	GPICON = 0xaaaaaaaa;

	//LCD VD[16-23],HSYNC,VSYNC,VDEN,VCLK
	GPJCON = 0xaaaaaaaa;

	//set LCD_Backlight en, GPF14 set as input ,(PWM dimming is not used now)
	GPFCON &= ~(0x3 << 28);
	GPFCON |= (0 << 28);
}

/*1. MOFPCON: SEL_BYPASS[3] value @ 0x7410800C must be set as ‘0’(normal mode) instead of ‘1’(by-pass
 * mode).
 * 2. SPCON: LCD_SEL[1:0] value @ 0x7F0081A0 must be set as ‘00’ to use Host I/F Style or as ‘01’ to use RGB
 * I/F Style
 */
void lcd_interface_init(void)
{
	//set normal mode
	MIFPCON &= ~(1 << 3);

	//use RGB I/F style
	SPCON &= ~(0x3 << 0);
	SPCON |= 0x01;
}

void lcd_control_init(void)
{
	//RGB I/F,RGB mode,LCD sourse select HCLK
	VIDCON0 &= ~((0x3 << 26) | (0x3 << 17) | (0xff << 6) | (0x3 << 2));
	//vclk== HCLK / (CLKVAL+1) = 133/14.7 = 9MHz
	VIDCON0 |= ((14 << 6) | (1 << 4));
	
	//get video data at VCLK falling edge;HSYNC and VSYNC is active-low,EN is active-high
	VIDCON1 &= ~((1 << 7) | (1 << 4));
	VIDCON1 |= ((1 << 6) | (1 << 5));

	//video time control
	VIDTCON0 = ((VSPW << 0) | (VFPD << 8) | (VBPD << 16));
	VIDTCON1 = ((HSPW << 0) | (HFPD << 8) | (HBPD << 16));
	VIDTCON2 = ((HOZVAL << 0) | (LINEVAL << 11));

	//set window0
	WINCON0 &= ~(0b1111 << 2);
	WINCON0 |= (0b1011 << 2);  //unpacked 24 BPP (non-palletized R:8-G:8-B:8 )
	//WINCON0 = (0x05<<2);	//16 BPP (non-palletized, R: 5-G:6-B:5 )

	//set coordinate for left top pixel of OSD image
	VIDOSD0A = ((LeftTopX_F_Win0 << 11) | (LeftTopY_F_Win0 << 0));
	//coordinate for right bottom pixel of OSD image
	VIDOSD0B = ((RightBotX_F_Win0 << 11) | (RightBotY_F_Win0 << 0));
	//set window0 size
	VIDOSD0C = OSDSIZE_Win0;

	//set frame buff start of address and end of address
	VIDW00ADD0B0 = (unsigned long)LCD_FRAME_BUFF;
	VIDW00ADD1B0 = (((unsigned long)LCD_FRAME_BUFF + (LINEVAL + 1)*(HOZVAL + 1)*4 ) & 0xffffff);

	//set virtual screen
	VIDW00ADD2 = ((OFFSIZE_F_Win0 << 13) | (PAGEWIDTH_F_Win0 << 0));
	
	//set DITHMODE 
	DITHMODE = 1;
	
	//set background color
	WIN0MAP = 0x0; //no backgrond color
	
	//set Window Palette 
	WPALCON = 1; //window0 24-bit ( 8:8:8 )

}

void lcd_enable(void)
{
	VIDCON0 |= ((1 << 1) | (1 << 0)); //enable LCD
	WINCON0 |= (1 << 0); //enable window0
}


void lcd_disable(void)
{
	VIDCON0 &= ~((1 << 1) | (1 << 0));
	WINCON0 &= ~(1 << 0);
}

void lcd_init(void)
{
	//LCD GPIO init
	lcd_gpio_init();

	//LCD interface init
	lcd_interface_init();

	//LCD control init
	lcd_control_init();

	//LCD enable
	lcd_enable();
}

// Point 
void point(unsigned int x, unsigned int y, unsigned int color)
{
	unsigned int red, green, blue;

	red = ((color >> 16) & 0xff);
	green = ((color >> 8) & 0xff); 
	blue = ((color >> 0) & 0xff);

	LCD_FRAME_BUFF[y][x] = (unsigned long)color;
	//LCD_FRAME_BUFF[y][x] = (unsigned long)((red << 16) | (green << 8) (blue << 0));
}

//const unsigned char gImage_image[45100]
void display_image(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const unsigned char image[])
{
	int x0, y0;

	int red, green, blue;

	int color;

	unsigned char *p = (unsigned char *)image;

	for(y0 = y; y0 < (y + height); y0++) {
		for (x0 = x; x0 < (x + width); x0++) {
			blue = *p++;
			green = *p++;
			red = *p++;

			color = ((red << 16) | (green << 8) | (blue << 0));

			point(x0,y0,color);
		}
	}
}

void pure_color_on(unsigned color)
{
	WIN0MAP &= ~(0xffffff);
	WIN0MAP |= (color & 0xffffff);
	WIN0MAP |= (1 << 24);
}

void pure_color_off(void)
{
	WIN0MAP &= ~(0x1 << 24);
}

void clear_screen(unsigned int color)
{
	unsigned int x, y;

	for(y = 0; y <= LINEVAL; y++) {
		for(x = 0; x <=HOZVAL; x++) {
			point(x,y,color);
		}
	}
}

extern const unsigned char snow_image[];
void lcd_test(void)
{
	int x;
	
	for (x = 0; x < 480; x++) {
		point(x++,75,0xff0000);
	}

	clear_screen(0x0);

	display_image(225,121,30,30,snow_image);

}
