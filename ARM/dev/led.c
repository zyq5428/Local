#define GPMCON (*((volatile unsigned long *) 0x7f008820))
#define GPMDAT (*((volatile unsigned long *) 0x7f008824))
#define GPMPUD (*((volatile unsigned long *) 0x7f008828))

void led_init(void)
{
	GPMCON = 0x00001111;
}

void led_on(void)
{
	GPMDAT &= ~(0b111);
}

void led_off(void)
{
	GPMDAT |= 0b111;
}

void led_xor(void)
{
	GPMDAT ^= 0b111;
}


void led4_on(void)
{
	GPMDAT &= ~(0b1000);
}

void led4_off(void)
{
	GPMDAT |= 0b1000;
}

void led4_xor(void)
{
	GPMDAT ^= 0b1000;
}
