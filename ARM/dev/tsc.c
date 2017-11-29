#define ADCCON		(*((volatile unsigned long *)0x7E00B000))

#define VIC1INTENABLE   (*((volatile unsigned long *)0x71300010))
#define VIC1INTENCLEAR  (*((volatile unsigned long *)0x71300014))
#define VIC1VECTADDR30  (*((volatile unsigned long *)0x71300178))
#define VIC0ADDRESS	(*((volatile unsigned long *)0x71200f00))
#define VIC1ADDRESS     (*((volatile unsigned long *)0x71300f00))

#define ADCTSC          (*((volatile unsigned long *)0x7E00B004))
#define ADCDLY          (*((volatile unsigned long *)0x7E00B008))
#define ADCDAT0         (*((volatile unsigned long *)0x7E00B00C))
#define ADCDAT1         (*((volatile unsigned long *)0x7E00B010))

#define ADCCLRINT       (*((volatile unsigned long *)0x7E00B018))
#define ADCCLRINTPNDNUP (*((volatile unsigned long *)0x7E00B020))


int x_position, y_position;

void ts_handler()
{
	__asm__ volatile (
		"sub lr, lr, #4\n"
		"stmfd sp!, {r0-r12, lr}\n"
		:
		:
	);
	
	//start Auto Sequential measurement of X-position, Y-position.
	ADCCON |= 1;
	ADCTSC |= (1 << 2);

	//wait conversion complete
	while(!(ADCCON & (1 << 15))) ;

	//get x,y position
	x_position = (ADCDAT0 & 0x3ff);
	y_position = (ADCDAT1 & 0x3ff);
	printf("x is %d, y is %d\n\r", x_position, y_position);

	// INTERRUPT CLEAR(dowm)	
	ADCCLRINT = 0;
	ADCCLRINTPNDNUP = 0;

	// into wait interrupt(up)
	ADCTSC = 0xd3;
	ADCTSC |= (1 << 8);

	//clear interrupt(up)
	ADCCLRINT = 0;
	ADCCLRINTPNDNUP = 0;

	//into wait interrupt(dowm)
	ADCTSC = 0xd3;

	//exit interrupt
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
	
	__asm__ volatile (
		"ldmfd sp!, {r0-r12, pc}^ \n"
		:
		:
	);

}

void ts_init(void)
{
	//set adc clk source:enable A/D converter prescaler;
	ADCCON &= ~((1 << 16) | (1 << 14) | (0xff << 6) | (1 << 2) | (1 << 1) | (1 << 0));
	//set adc clk = PCLK / (PRSCVL + 1), ADC frequency should be set less than PCLK by 5 times.This A/D converter is designed to operate at maximum 5MHz clock
	ADCCON |= ((1 << 14) | (65 << 6));	//adc clk = 66.5MHz / (65+1) = 1MHz

	//set ADC Start or Interval Delay
	ADCDLY = 0xffff;

	//set interrupt:TC interrupt enable;adc interrupt disable.
	VIC1INTENABLE |= (1 << 30);
	VIC1INTENCLEAR &= ~(1 << 31);

	VIC1VECTADDR30 = (int)ts_handler;

	ADCCLRINT = 0;
	ADCCLRINTPNDNUP = 0;
	//into wait interrupt(dowm)
	ADCTSC = 0xd3;
}
