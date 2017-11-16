#define TCFG0          (*((volatile unsigned long *)0x7F006000))
#define TCFG1          (*((volatile unsigned long *)0x7F006004))
#define TCON           (*((volatile unsigned long *)0x7F006008))
#define TCNTB2         (*((volatile unsigned long *)0x7F006024))
#define TCNTO2         (*((volatile unsigned long *)0x7F00602C))
#define TINT_CSTAT     (*((volatile unsigned long *)0x7F006044))

void timer_2_init(void)
{
	/*Timer input clock Frequency = 66MHz / ( {65 + 1} ) / 1 = 1us */
	
	/*set Prescaler 1 value for Timer 2, 3 and 4 is 65*/
	TCFG0 &= ~(0xff << 8);
	TCFG0 |= (65 << 8);

	/*Select Mux input for PWM Timer 2 : 1/1 */
	TCFG1 &= ~(0xf << 8);

	/*clear Timer Control Register */
	TCON &= ~(0xf << 12);
}

void timer_2_timing(unsigned long usec)
{
	int n;

	n = usec;
	
	/*clear timer2 interrupt statu */
	TINT_CSTAT |= (0x1 << 7);
	
	/*Timer 2 Auto Reload on */
	TCON |= (0x1 << 15);
		
	/*load count value */
	TCNTB2 = n;
	
	/*Timer 2 Manual Update */
	TCON |= (0x1 << 13);
	
	/*Timer 2 Interrupt Enable. */
	TINT_CSTAT |= (1 << 2);
	
	/* Start Timer 2 */
	TCON |= (1 << 12);

	/*end Timer 2 Manual Update */
	TCON &= ~(0x1 << 13); 

}

int timer_2_int_isr(void)
{
	led_xor();

	/*Timer 2 Interrupt disable. */
	TINT_CSTAT &= ~(1 << 2);

	/*Timer 2 Auto Reload off */
	TCON &= ~(0x1 << 15);

	/* end Timer 2 */
	TCON &= ~(1 << 12);

	return 0;
}

void udelay(unsigned long usec)
{
	int n;

	n = usec;
	
	/*clear timer2 interrupt statu */
	TINT_CSTAT |= (0x1 << 7);
	
	/*Timer 2 Auto Reload on */
	TCON |= (0x1 << 15);
	
	/*load count value */
	TCNTB2 = n;

	/*Timer 2 Manual Update */
	TCON |= (0x1 << 13);
	
	/*Timer 2 Interrupt disable. */
	TINT_CSTAT &= ~(1 << 2);

	/* Start Timer 2 */
	TCON |= (1 << 12);

	/*end Timer 2 Manual Update */
	TCON &= ~(0x1 << 13); 

	while (!(TINT_CSTAT & (0x1 << 7))) ;

	/*clear timer2 interrupt statu */
	TINT_CSTAT |= (0x1 << 7);

	/*Timer 2 Auto Reload off */
	TCON &= ~(0x1 << 15);
	
	/* end Timer 2 */
	TCON &= ~(1 << 12);
}

void mdelay(unsigned long msec)
{
	int n;
	n = msec * 1000;
	udelay(n);
}

/* sec < 4294 */
void sdelay(unsigned long sec)
{
	int n;
	n = sec * 1000000;
	udelay(n);
}

void timer_2_TCNTO_print(void)
{
	int count;
	count = TCNTO2;
	printf("\n TCNTO2 is %d!\n\r", TCNTO2);
}
