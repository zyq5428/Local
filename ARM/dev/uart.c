#define GPACON		(*((volatile unsigned long *)0x7f008000))
#define ULCON0  	(*((volatile unsigned long *)0x7f005000))
#define UCON0		(*((volatile unsigned long *)0x7f005004))
#define UFCON0          (*((volatile unsigned long *)0x7f005008))
#define UMCON0          (*((volatile unsigned long *)0x7f00500c))

#define UTRSTAT0        (*((volatile unsigned long *)0x7f005010))
#define UFSTAT0         (*((volatile unsigned long *)0x7f005018))
#define UTXH0           (*((volatile unsigned char *)0x7f005020))
#define URXH0           (*((volatile unsigned char *)0x7f005024))

#define UBRDIV0		(*((volatile unsigned short *)0x7f005028))
#define UDIVSLOT0	(*((volatile unsigned short *)0x7f00502c))

#define PCLK	66500000
#define BAUD	115200

void uart_init(void)
{
	//set GPIO to enable UART,GPA0 is RXD,GPA1 is TXD
	GPACON &= ~(0xff);
	GPACON |= ((0b10 << 0) | (0b10 << 4));
	
	//set frame format:8 bits data,one stop bit,No parity
	ULCON0 = ((0b11 << 0) | (0b0 << 2) | (0b000 << 3) | (0b0 << 6)); 
	
	//set UART model:polling mode
	UCON0 |= ((0b01 << 0) | (0b01 << 2));
	
	UFCON0 = 0x01;

	UMCON0 = 0x0;

	//set UART baud rate, bps=115200
	UBRDIV0 = (int)(PCLK / (BAUD * 16) -1);
	UDIVSLOT0 = 0x1; 
	
	//set UART CLK : PCLK
	UCON0 |= (0x00 << 10);
}

void uart_put_char(unsigned char c)
{
	while (UFSTAT0 & (1 << 14)) ;	//if TX FIFO FULL,wait
	UTXH0 = c;
}

unsigned char uart_get_char(void)
{
	unsigned char c;

	while ((UFSTAT0 & 0x7f) == 0) ;
	c = URXH0;

	if ((c == 0x0d) || (c == 0x0a)) {
		uart_put_char(0x0d);
		uart_put_char(0x0a);
	}
	else
		uart_put_char(c);

	return c;
}

