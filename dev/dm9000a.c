#include "type.h"

#define CONFIG_DM9000_BASE	0x18000300 /*XMOCSN1*/
#define DM9000_DATA		0x18000304 /*ADDR2*/
#define DM9000_IO		CONFIG_DM9000_BASE
#define CONFIG_DM9000_USE_16BIT	1
#define DM9000_BASE		CONFIG_DM9000_BASE
#define DM9000_PPTR		(*((volatile unsigned long *)(DM9000_IO)))
#define DM9000_PDATA		(*((volatile unsigned long *)(DM9000_DATA)))

#define SROM_BW		(*((volatile unsigned long *)0x70000000))
#define SROM_BC1	(*((volatile unsigned long *)0x70000008))

/* ------------------------------------------------------------------------- */
#define DM9000_Tacs	(0x0)	// 0clk		address set-up
#define DM9000_Tcos	(0x4)	// 4clk		chip selection set-up
#define DM9000_Tacc	(0xE)	// 14clk	access cycle
#define DM9000_Tcoh	(0x1)	// 1clk		chip selection hold
#define DM9000_Tah	(0x4)	// 4clk		address holding time
#define DM9000_Tacp	(0x6)	// 6clk		page mode access cycle
#define DM9000_PMC	(0x0)	// normal(1data)page mode configuration
/* ------------------------------------------------------------------------- */

#define GPNCON		(*((volatile unsigned long *)0x7F008830))

void dm9000_cs_init(void)
{
	 //Data bus width control for Memory Bank1
	 SROM_BW &= (~(0xf<<4));
	 SROM_BW |= (0x1<<4);

	//SROM BANK CONTROL REGISTER
	SROM_BC1 = ((DM9000_Tacs<<28)+(DM9000_Tcos<<24)+(DM9000_Tacc<<16)+(DM9000_Tcoh<<12)+(DM9000_Tah<<8)+(DM9000_Tacp<<4)+(DM9000_PMC));
}

void dm9000_int_init(void)
{
	//set GPN7 as EINT7
	GPNCON &= ~(0x3 << 14);
	GPNCON |= (0x2 << 14);
}

static void iow(unsigned short reg, unsigned short data)
{
	DM9000_PPTR = reg;
	DM9000_PDATA = data;
}

static unsigned char ior(int reg)
{
        DM9000_PPTR = reg;
        return DM9000_PDATA;
}

void dm9000_reset(void)
{

}

void dm9000_init()
{
	//chip select dm9000
	dm9000_cs_init();

	//dm9000 interrupt init
	dm9000_int_init();

	//reset dm9000
	dm9000_reset();
	
	//capture dm9000
	//
	//mac init
	//
	//fill mac address
	//
	//activate dm9000
	//
}

void dm9000_tx()
{

}

void dm9000_rx()
{

}
