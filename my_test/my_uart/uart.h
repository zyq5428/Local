#ifndef _UART_H
#define _UART_H

#define PCLK	66500000
#define bps	115200
#define DIV_VAL	(PCLK / (bps * 16) - 1)
#define UBRDIVn	(int)DIV_VAL
#define UDIVSLOTn	(int)((DIV_VAL - UBRDIVn) * 16)



#define GPBCON	0x7F008020
#define GPBDAT	0x7F008024
#define GPB2_3_SIZE	(GPBDAT - GPBCON + 4)

#define _ULCON3	0x7F005C00
#define _UINTM3	0x7F005C38
#define UART3_BASE	_ULCON3
#define UART3_SIZE	(_UINTM3 - _ULCON3 + 4)


#ifdef NO_POINT

#define ULCON3 0x0
#define UCON3 0x4
#define UFCON3 0x8
#define UTRSTAT3 0x10
#define UERSTAT3 0x14
#define UFSTAT3 0x18
#define UTXH3 0x20
#define URXH3 0x24
#define UBRDIV3 0x28
#define UDIVSLOT3 0x2C
#define UINTP3 0x30
#define UINTSP3 0x34
#define UINTM3 0x38

#else
	
#define ULCON3 0x0
#define UCON3 0x1
#define UFCON3 0x2

#define UTRSTAT3 0x4
#define UERSTAT3 0x5
#define UFSTAT3 0x6

#define UTXH3 0x8
#define URXH3 0x9
#define UBRDIV3 0xa
#define UDIVSLOT3 0xb
#define UINTP3 0xc
#define UINTSP3 0xd
#define UINTM3 0xe

#endif

#endif /* _UART_H */
