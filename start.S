.text
.global _start
_start:
	b reset
	ldr pc, _undifined_instrution
	ldr pc, _software_interrupt
	ldr pc, _prefetch_abort
	ldr pc, _data_abort
	ldr pc, _not_used
	ldr pc, _irq
	ldr pc, _fiq

_undifined_instrution: .word undifined_intruction
_software_interrupt: .word software_interrupt
_prefetch_abort: .word prefetch_abort
_data_abort: .word data_abort
_not_used: .word not_used
_irq: .word irq
_fiq: .word fiq

undifined_intruction:
	nop

software_interrupt:
	nop

prefetch_abort:
	nop

data_abort:
	nop

not_used:
	nop

irq:
	nop

fiq:
	nop

reset:
	bl set_svc
	bl peri_port_setup
	bl disable_watchdog
	bl disable_interrupt
	bl flush_cache
	bl disable_mmu_cache
	bl system_clock_init
	bl men_ctrl_asm_init
	bl stack_init
	bl nand_init

	bl copy_to_ram
@	bl iram_to_ram
	bl clear_bss
	ldr pc, _uboot_main
@	bl led_on_asm

_uboot_main:
	.word uboot_main

set_svc:
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0xd3
	msr cpsr, r0
	mov pc, lr

peri_port_setup:
	ldr r0, =0x70000000
	orr r0, r0, #0x13
	mcr p15, 0, r0, c15, c2, 4       @ 256M(0x70000000-0x7fffffff)
	mov pc, lr

#define WTCON (0x7e004000)
disable_watchdog:
	ldr r0, =WTCON
	mov r1, #0x0
	str r1, [r0]
	mov pc, lr

#define ELFIN_VIC0_BASE_ADDR (0x71200000)
#define ELFIN_VIC1_BASE_ADDR (0x71300000)
disable_interrupt:
	ldr r0, =ELFIN_VIC0_BASE_ADDR
	ldr r1, =ELFIN_VIC1_BASE_ADDR
	mvn r3, #0
	str r3, [r0, #0x14]
	str r3, [r1, #0x14]
	mov pc, lr

flush_cache:
	mov r0, #0
	mcr p15, 0, r0, c7, c7, 0
	mcr p15, 0, r0, c8, c7, 0
	mov pc, lr

disable_mmu_cache:
	mrc p15, 0, r0, c1, c0, 0
	bic r0, r0, #0x00000007
	mcr p15, 0, r0, c1, c0, 0
	mov pc, lr

#define SYS_CTL_BASE 0x7e00f000
#define OTHERS_OFFSET 0x900

#define CONFIG_SYNC_MODE

#define CLK_DIV0_OFFSET 0x20
#define CLK_DIV_VAL ((3<<12)|(1<<9)|(1<<8)|(1<<4)|(0<<0))
#define APLL_VAL ((1<<31)|(266<<16)|(3<<8)|(1<<0))
#define MPLL_VAL ((1<<31)|(266<<16)|(3<<8)|(1<<0))
#define APLL_CON_OFFSET 0x0c
#define MPLL_CON_OFFSET 0x10
#define CLK_SRC_OFFSET 0x1c
system_clock_init:
	ldr r0, =SYS_CTL_BASE

#ifdef CONFIG_SYNC_MODE /* SYNC Mode */
	ldr r1, [r0, #OTHERS_OFFSET]
	mov r2, #0x40
	orr r1, r1, r2
	str r1, [r0, #OTHERS_OFFSET]

	nop
	nop
	nop
	nop
	nop

	ldr r2, =0x80
	orr r1, r1, r2
	str r1, [r0, #OTHERS_OFFSET]

check_syncack:
	ldr	r1, [r0, #OTHERS_OFFSET]
	ldr	r2, =0xf00
	and	r1, r1, r2
	cmp	r1, #0xf00
	bne	check_syncack

#else /* ASYNC Mode */
	nop
	nop
	nop
	nop
	nop
	ldr r1, [r0, #OTHERS_OFFSET]
	bic r1, r1, #0xc0
	orr r1, r1, #0x40
	str r1, [r0, #OTHERS_OFFSET]

wait_for_async:
	ldr r1, [r0, #OTHERS_OFFSET]
	and r1, r1, #0xf00
	cmp r1, #0x0
	bne wait_for_async

	ldr r1, [r0, #OTHERS_OFFSET]
	bic r1, r1, #0x40
	str r1, [r0, #OTHERS_OFFSET]
#endif

	ldr r1, [r0, #CLK_DIV0_OFFSET]
	bic r1, r1, #0x30000
	bic r1, r1, #0xff00
	bic r1, r1, #0xff
	ldr r2, =CLK_DIV_VAL
	orr r1, r1, r2
	str r1, [r0, #CLK_DIV0_OFFSET]

	ldr r1, =APLL_VAL
	str r1, [r0, #APLL_CON_OFFSET]
	ldr r1, =MPLL_VAL
	str r1, [r0, #MPLL_CON_OFFSET]

	ldr r1, [r0, #CLK_SRC_OFFSET]
	ldr r2, =0x7
	orr r1, r1, r2
	str r1, [r0, #CLK_SRC_OFFSET]

	mov pc, lr
iram_to_ram:
	ldr r0, =0x0c000000
	ldr r1, =0x50008000
	add r3, r0, #1024*4

copy_loop:
	ldr r2, [r0], #4
	str r2, [r1], #4
	cmp r0, r3
	bne copy_loop

	mov pc, lr


copy_to_ram:
	mov r0, #0
	ldr r1, =_start
	ldr r2, =bss_end
	sub r2, r2, r1

	mov ip, lr
	bl nand_to_ram
	mov lr, ip

	mov pc, lr

stack_init:
	msr cpsr_c, #0xd2
	ldr sp, =0x53000000	//init R13_IRQ

	msr cpsr_c, #0xd3
	ldr sp, =0x54000000	//init R13_SVC
	mov pc, lr

clear_bss:
	ldr r0, =bss_start
	ldr r1, =bss_end
	cmp r0, r1
	moveq pc, lr

clbss_1:
	str r2, [r0]
	add r0, r0, #4
	cmp r0, r1
	bne clbss_1

	mov pc, lr


#define GPMCON 0x7f008820
#define GPMDAT 0x7f008824
#define GPMPUD 0x7f008828
.global led_on_asm
led_on_asm:
	ldr r0, =GPMCON
	ldr r1, =0x00001111
	str r1, [r0]

	ldr r0, =GPMDAT
	ldr r1, =0b001010
	str r1, [r0]

	mov pc, lr


