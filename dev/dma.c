#define SDMAC0_BASE		(*((volatile unsigned long *)0x7db00000))

#define SDMA_SEL	        (*((volatile unsigned long *)0x7e00f110))
#define DMACIntTCClear		(*((volatile unsigned long *)0x7db00008))
#define DMACIntErrClr		(*((volatile unsigned long *)0x7db00010))
#define DMACConfiguration	(*((volatile unsigned long *)0x7db00030))
#define DMACSync	        (*((volatile unsigned long *)0x7db00034))
#define DMACC0SrcAddr   	(*((volatile unsigned long *)0x7db00100))
#define DMACC0DestAddr  	(*((volatile unsigned long *)0x7db00104))
#define DMACC0Control0		(*((volatile unsigned long *)0x7db0010c))
#define DMACC0Control1          (*((volatile unsigned long *)0x7db00110))
#define DMACC0Configuration	(*((volatile unsigned long *)0x7db00114))


#define UTXH0           ((volatile unsigned long *)0x7f005020)

char src[100] = "\n\rDMA transfer test!\n\r";

void dma_init()
{
	//Decide whether use secure DMAC(SDMAC) or general DMAC(DMAC). 
	SDMA_SEL = 0x0;
	
	//enable DMA controller
	DMACConfiguration = 0x1;

	//Select a free DMA channel with the priority needed.
	//It's already set up in the SDMA_SEL register
	
	//Clear any pending interrupts on the channel to be used by writing to the DMACIntTCClr and DMACIntErrClr
	//registers.
	DMACIntTCClear = 0x0f;
	DMACIntErrClr = 0x0f;
	
	//Write the source address into the DMACCxSrcAddr register.
	DMACC0SrcAddr = (unsigned int)src;

	//Write the destination address into the DMACCxDestAddr register.
	DMACC0DestAddr = (unsigned int)UTXH0;

	//Write the address of the next LLI into the DMACCxLLI register. 
	//Now don't have to
	
	//Write the control information into the DMACCxControl register.
	
	//Destination AHB master select AHB 2,Source increment,Destination fixed,Source AHB master select AHB master 1,transfer width 8bit.1 size burst transfer
	DMACC0Control0 = ((0 << 31) | (0 << 27) | (1 << 26) | (1 << 25) | (0 << 24));

	//set Transfer size
	DMACC0Control1 = 0x64;
	
	//allow DMA requests, interrupt mask,memory-to-peripheral,
	DMACC0Configuration = ((1 << 6) | (0 << 11) | (1 << 14) | (1 << 15));
}

void dma_start(void)
{
	DMACC0Configuration |= 0x1;
}

