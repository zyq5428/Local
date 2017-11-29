#include "interrupt.h"

#define GPNDAT		(*((volatile unsigned long *)0x7f008834))
#define GPNPUD  	(*((volatile unsigned long *)0x7f008838))

extern const unsigned char snow_image[];

void eint0_3_handle(void)
{
	__asm__ volatile (
		"sub lr, lr, #4\n"
		"stmfd sp!, {r0-r12, lr}\n"
		:
		:
	);
	
	printf("\n\r into ext_interrupt 0~3 \n\r");
	
	switch ( GPNDAT & 0x3 ) {
		case 0x2:		//key0
			led_off();
			clear_screen(0x0);
			display_image(0,0,30,30,snow_image);
			 break;
		case 0x1:		//key1
			led_on(); 
			clear_screen(0x0);
			display_image(450,242,30,30,snow_image);
			break;
		default:
			break;
	}

	EINT0PEND = ~0x0;
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
	
	printf("\n\r exit ext_interrupt 0~3 \n\r");

	__asm__ volatile (
		"ldmfd sp!, {r0-r12, pc}^ \n"
		:
		:
	);

}

void eint4_11_handle(void)
{
	__asm__ volatile (
		"sub lr, lr, #4\n"
		"stmfd sp!, {r0-r12, lr}\n"
		:
		:
	);
	
	int flag;
	
	//printf("\n into ext_interrupt 4~11 \n\r");
	
	switch ( GPNDAT & (0x1 << 7) ) {
		case 0b10000000:		//Eint7 is hight
			flag = get_arp_flag();
			if (flag) {
				dm9000_int_isr();
			}
			break;
		case 0x0:		//Eint7 is low
			
			break;
		default:
			break;
	}

	EINT0PEND = ~0x0;
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
	
	//printf("\n\r exit ext_interrupt 4~11 \n\r");

	__asm__ volatile (
		"ldmfd sp!, {r0-r12, pc}^ \n"
		:
		:
	);

}

void timer_2_handle(void)
{
	__asm__ volatile (
		"sub lr, lr, #4\n"
		"stmfd sp!, {r0-r12, lr}\n"
		:
		:
	);
	
	int n;
	
	n = TINT_CSTAT;
	
	printf("\n\r into timer_2 interrupt \n\r");
	
	printf("\n\r TINT_CSTAT is : %d \n\r", n);
	
	/*clear timer2 interrupt statu */
	TINT_CSTAT |= (0x1 << 7);
	
	timer_2_int_isr();

	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;
	
	printf("\n\r exit timer_2 interrupt \n\r");

	__asm__ volatile (
		"ldmfd sp!, {r0-r12, pc}^ \n"
		:
		:
	);

}

void irq_init(void)
{
	//set GPN0,1 Falling edge triggered
	EINT0CON0 = (0x2 << 0);
	
	//set GPN 7 High level triggered (for eth)
	EINT0CON0 &= ~(0x7 << 12);
	EINT0CON0 |= (0x1 << 12);
	

	//chear EINT0MASK
	EINT0MASK = 0x0;

	//enable EINT
	VIC0INTENABLE = ((0x1 << 0) | (0x1 << 1) | (0x1 << 25));

	//External interrupt 0 ~ 3
	VIC0VECTADDR0 = (int)eint0_3_handle;
	
	//External interrupt 4 ~ 11
	VIC0VECTADDR1 = (int)eint4_11_handle;
	
	//Timer 2 interrupt
	VIC0VECTADDR25 = (int)timer_2_handle;

	__asm__ volatile (
		"mrc p15,0,r0,c1,c0,0\n"
		"orr r0,r0,#(1<<24)\n"
		"mcr p15,0,r0,c1,c0,0\n"

		"mrs r0,cpsr\n"
		"bic r0, r0, #0x80\n"
		"msr cpsr_c, r0\n"
		:
		:
	);


}
