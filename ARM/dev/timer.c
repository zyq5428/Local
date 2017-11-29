#define TCFG0          (*((volatile unsigned long *)0x7F006000))
#define TCFG1          (*((volatile unsigned long *)0x7F006004))
#define TCON           (*((volatile unsigned long *)0x7F006008))
#define TCNTB2         (*((volatile unsigned long *)0x7F006024))
#define TCMPB2         (*((volatile unsigned long *)0x7F006028))
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
}

void timer_2_timing(unsigned long usec)
{
	int n,m,f;

	n = usec;
	
	/*Timer 2 Interrupt Enable. */
	TINT_CSTAT |= (1 << 2);
	
	//printf("\n\r Timer 2 Interrupt Enable\n\r");
	
	/*clear Timer Control Register */
	TCON &= ~(0xf << 12);
	
	//printf("\n\r clear Timer Control Register\n\r");
	
	/*clear timer2 interrupt statu */
	//TINT_CSTAT |= (0x1 << 7);
	
	/*f = TCNTB2;
	printf("\n\r TCNTB2 is : %d \n\r", f);
	m = TCNTO2;
	printf("\n\r TCNTO2 is : %d \n\r", m);*/
	
	/*load count value */
	TCNTB2 = n;
	
	/*printf("\n\r load count value\n\r");

	f = TCNTB2;
	printf("\n\r TCNTB2 is : %d \n\r", f);
	m = TCNTO2;
	printf("\n\r TCNTO2 is : %d \n\r", m);*/
	
 	/*Timer 2 Auto Reload on */
	TCON |= (0x1 << 15);
	
	/*printf("\n\r Timer 2 Auto Reload on\n\r");
	
	f = TCNTB2;
	printf("\n\r TCNTB2 is : %d \n\r", f);
	m = TCNTO2;
	printf("\n\r TCNTO2 is : %d \n\r", m);*/
	
	/*Timer 2 Manual Update */
	TCON |= (0x1 << 13);
	
	/*printf("\n\r Timer 2 Manual Update\n\r");
	
	f = TCNTB2;
	printf("\n\r TCNTB2 is : %d \n\r", f);
	m = TCNTO2;
	printf("\n\r TCNTO2 is : %d \n\r", m);*/
	
	/*end Timer 2 Manual Update */
	TCON &= ~(0x1 << 13);
	
	/* Start Timer2 */
	TCON |= (1 << 12);
	
	/*printf("\n\r Start Timer2\n\r");
	
	f = TCNTB2;
	printf("\n\r TCNTB2 is : %d \n\r", f);
	m = TCNTO2;
	printf("\n\r TCNTO2 is : %d \n\r", m);*/

	/*Timer 2 Auto Reload off */
	TCON &= ~(0x1 << 15);
	
	/*printf("\n\r Timer 2 Auto Reload off\n\r");
	
	f = TCNTB2;
	printf("\n\r TCNTB2 is : %d \n\r", f);
	m = TCNTO2;
	printf("\n\r TCNTO2 is : %d \n\r", m);*/

}

void timer_2_ms(unsigned long msec)
{
	int n;
	n = msec * 1000;
	timer_2_timing(n);
}

/* sec < 4294 */
void timer_2_s(unsigned long sec)
{
	int n;
	n = sec * 1000000;
	timer_2_timing(n);
}

int timer_2_int_isr(void)
{
	led_xor();
	
	//input_time();

	/* end Timer 2 */
	TCON &= ~(1 << 12);
	
	/*Timer 2 Interrupt disable. */
	TINT_CSTAT &= ~(1 << 2);
	
	return 0;
}

void udelay(unsigned long usec)
{
	int n;

	n = usec;
	
	/*Timer 2 Interrupt disable. */
	TINT_CSTAT &= ~(1 << 2);
	
	/*clear Timer Control Register */
	TCON &= ~(0xf << 12);
	
	/*clear timer2 interrupt statu */
	TINT_CSTAT |= (0x1 << 7);
	
	/*load count value */
	TCNTB2 = n;
	
	/*Timer 2 Auto Reload on */
	TCON |= (0x1 << 15);

	/*Timer 2 Manual Update */
	TCON |= (0x1 << 13);
	
	/*end Timer 2 Manual Update */
	TCON &= ~(0x1 << 13); 

	/* Start Timer 2 */
	TCON |= (1 << 12);
	
	/*Timer 2 Auto Reload off */
	TCON &= ~(0x1 << 15);

	while (!(TINT_CSTAT & (0x1 << 7))) ;

	/*clear timer2 interrupt statu */
	TINT_CSTAT |= (0x1 << 7);
	
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
