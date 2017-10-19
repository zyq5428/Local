#define GPNDAT	((volatile unsigned long *)0x7f008834)
#define GPNPUD  ((volatile unsigned long *)0x7f008838)

#define EINT0CON0	((volatile unsigned long *)0x7f008900)
#define EINT0MASK       ((volatile unsigned long *)0x7f008920)
#define EINT0PEND       ((volatile unsigned long *)0x7f008924)

#define VIC0INTENABLE   ((volatile unsigned long *)0x71200010)
#define VIC0VECTADDR0   ((volatile unsigned long *)0x71200100)
#define VIC0ADDRESS	((volatile unsigned long *)0x71200f00)
#define VIC1ADDRESS     ((volatile unsigned long *)0x71300f00)

void key1_handle(void)
{
	__asm__ volatile (
		"sub lr, lr, #4\n"
		"stmfd sp!, {r0-r12, lr}\n"
		:
		:
	);
	
	switch ( (*GPNDAT) & 0x3 ) {
		case 0x2:		//key0
			led_off(); break;
		case 0x1:		//key1
			led_on(); break;
		default:
			break;
	}

	*(EINT0PEND) = ~0x0;
	*(VIC0ADDRESS) = 0;
	*(VIC1ADDRESS) = 0;

	__asm__ volatile (
		"ldmfd sp!, {r0-r12, pc}^ \n"
		:
		:
	);

}

void irq_init(void)
{
	//set GPN0,1 Falling edge triggered
	*(EINT0CON0) = 0x2;

	//chear EINT0MASK
	*(EINT0MASK) = 0x0;

	//enable EINT
	*(VIC0INTENABLE) = 0x1;

	*(VIC0VECTADDR0) = (int)key1_handle;

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
