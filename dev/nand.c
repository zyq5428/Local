#define NFCONF (*((volatile unsigned long *)0x70200000))
#define NFCONT (*((volatile unsigned long *)0x70200004))
#define NFCMMD (*((volatile unsigned long *)0x70200008))
#define NFSTAT (*((volatile unsigned long *)0x70200028))
#define NFADDR (*((volatile unsigned long *)0x7020000c))
#define NFDATA (*((volatile unsigned char *)0x70200010))


void select_chip(void)
{
	NFCONT &= ~(1 << 1);
}

void delselect_chip(void)
{
	NFCONT |= (1 << 1);
}

void clear_RnB(void)
{
	NFSTAT |= (1 << 4);
}

void send_command(unsigned long cmd)
{
	NFCMMD = cmd;
}

void wait_RnB(void)
{
	while (!(NFSTAT & 0x1)) ;
}

void send_addr(unsigned long addr)
{
	NFADDR = addr;
}

void nand_reset(void)
{
	select_chip();

	clear_RnB();

	send_command(0xff);

	wait_RnB();

	delselect_chip();
}

#define TACLS   1
#define TWRPH0  2
#define TWRPH1  1

void nand_init(void)
{		
	//init NFCONF
	NFCONF &= ~((7 << 12) | (7 << 8) | (7 << 4));
	NFCONF |= ((TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4));

	//init_NFCONT
	NFCONT = (1 | (1 << 1));

	//reset
	nand_reset();
}

void NandFlash_PageRead(unsigned long addr, unsigned char * buff)
{
	int i;
	//select nand flash
	select_chip();
	
	//clear R/B signal
	clear_RnB();
	//send command
	send_command(0x00);
	
	//send column address (2 cycle)
	send_addr(0x00);

	send_addr(0x00);

	//send row address (3 cycle)
	send_addr(addr & 0xff);

	send_addr((addr >> 8) & (0xff));

	send_addr((addr >> 16) & (0xff));

	//send command
	send_command(0x30);

	//wait R/B signal
	wait_RnB();
	
	//read data
	for( i = 0; i < 1024 * 4; i++) {
		buff[i] = NFDATA;
	}

	//cancel select nand flash
	delselect_chip();

}

/*void nand_to_ram(unsigned long start_addr, unsigned char* sdram_addr, int size)
{
	int i;
	for (i = start_addr >> 12; size > 0; i++) {
		NandFlash_PageRead(i,sdram_addr);
		size -= 4096;
		sdram_addr += 4096;
	}
}*/


void nand_to_ram(unsigned long start_addr, unsigned char* sdram_addr, int size)
{
	int i;

	for (i = 0; i < 4; i++, sdram_addr += 2048) {
		NandFlash_PageRead(i,sdram_addr);
	}

	size -=1024*8;

	for ( i=4; size > 0; ) {
		NandFlash_PageRead(i,sdram_addr);
		size -= 4096;
		sdram_addr += 4096;
		i++;
	}
}

int Erase_NandFlash(unsigned long addr)
{
	int status;
	
	select_chip();

	clear_RnB();
	
	//send command 0x60
	send_command(0x60);
	
	//send row address (3 cycle)
	send_addr(addr & 0xff);

	send_addr((addr >> 8) & (0xff));

	send_addr((addr >> 16) & (0xff));
	
	//send command 0xD0
	send_command(0xd0);
	
	//wait RnB
	wait_RnB();
	
	//send command 0x70
	send_command(0x70);
	
	//read status
	status = NFDATA;
	//
	delselect_chip();
	
	return status;
}

int NandFlash_PageWrite(unsigned long addr, unsigned char * buff)
{
	int i,status;
	
	//select chip
	select_chip();
	
	//clean RnB
	clear_RnB();
	
	//send command 0x80
	send_command(0x80);
	
	//send column address (2 cycle)
	send_addr(0x00);
	
	send_addr(0x00);
	
	//send row address (3 cycle)
	send_addr(addr & 0xff);

	send_addr((addr >> 8) & (0xff));

	send_addr((addr >> 16) & (0xff));
	
	//write data
	for( i = 0; i < 1024 * 4; i++) {
		NFDATA = buff[i];
	}

	//send command 0x10
	send_command(0x10);
	
	//wait RnB signal
	wait_RnB();
	
	//send read status command 0x70
	send_command(0x70);
	
	//read status
	status = NFDATA;
	//
	//delselect chip
	delselect_chip();
	
	return status;
}
