#define MMU_SECTION	(2 << 0)
#define MMU_CACHEABLE	(1 << 3)
#define MMU_BUFFERABLE  (1 << 2)
#define MMU_SPECIAL 	(1 << 4)
#define MMU_DOMAIN	(0 << 5)
#define MMU_MULL_ACCESS	(3 << 10)
#define MMU_SECDESC	(MMU_MULL_ACCESS | MMU_DOMAIN | MMU_SPECIAL | MMU_SECTION)
#define MMU_SECDESC_WB  (MMU_MULL_ACCESS | MMU_DOMAIN | MMU_SPECIAL | MMU_CACHEABLE | MMU_BUFFERABLE | MMU_SECTION)


void creat_page_table(void)
{
	unsigned long *ttb = (unsigned long *)0x50000000;

	unsigned long vaddr, paddr;

	vaddr = 0xA0000000;
	paddr = 0x7F000000;

	*(ttb + (vaddr >> 20)) = (paddr & 0xfff00000) | MMU_SECDESC;

	vaddr = 0x50000000;
	paddr = 0x50000000;

	while (vaddr < 0x54000000) {
		*(ttb + (vaddr >> 20)) = (paddr & 0xfff00000) | MMU_SECDESC_WB;
		vaddr +=0x100000;
		paddr +=0x100000;
	}


}

void mmu_enable(void)
{
	__asm__ volatile (
		"ldr r0, =0x50000000\n"
		"mcr p15, 0, r0, c2, c0, 0\n"

		"mvn r0, #0\n"
		"mcr p15, 0, r0, c3, c0, 0\n"

		"mrc p15, 0, r0, c1, c0, 0\n"
		"orr r0, r0, #0x0001\n"
		"mcr p15, 0, r0, c1, c0, 0\n"
		:
		:
		
	);
}

void mmu_init()
{
	creat_page_table();
	mmu_enable();
}
