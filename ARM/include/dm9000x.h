/*
 * dm9000 Ethernet
 */


#define DM9KS_DWORD_MODE		1
#define DM9KS_BYTE_MODE		2
#define DM9KS_WORD_MODE		0

#define TRUE			1
#define FALSE			0

#define DM9KS_PKT_RDY		0x01	/* Packet ready to receive */

//#ifndef CONFIG_ARCH_MAINSTONE
//#pragma pack(push, 1)
//#endif

typedef struct _RX_DESC
{
        u8 rxbyte;
        u8 status;
        u16 length;
}RX_DESC;

typedef union{
        u8 buf[4];
        RX_DESC desc;
} rx_t;
//#ifndef CONFIG_ARCH_MAINSTONE
//#pragma pack(pop)
//#endif

enum DM9KS_PHY_mode {
        DM9KS_10MHD   = 0, 
        DM9KS_100MHD  = 1, 
        DM9KS_10MFD   = 4,
        DM9KS_100MFD  = 5, 
        DM9KS_AUTO    = 8, 
};

enum DM9KS_MAC_reg {
        DM9KS_NCR	=	0x00,	/* Network control Reg.*/
  DM9KS_NSR	=	0x01,	/* Network Status Reg.*/
  DM9KS_TCR	=	0x02,	/* TX control Reg.*/
  DM9KS_RXCR	=	0x05,	/* RX control Reg.*/
  DM9KS_BPTR	=	0x08,
  DM9KS_EPCR	=	0x0b,
  DM9KS_EPAR	=	0x0c,
  DM9KS_EPDRL	=	0x0d,
  DM9KS_EPDRH	=	0x0e,
  DM9KS_PAR	=	0x10,
  DM9KS_GPR	=	0x1f,	/* General purpose register */
  DM9KS_VID_L	=	0x28,
        DM9KS_VID_H	=	0x29,
  DM9KS_PID_L	=	0x2A,
  DM9KS_PID_H	=	0x2B,
  DM9KS_TCR2	=	0x2d,
  DM9KS_SMCR	=	0x2f, 	/* Special Mode Control Reg.*/
  DM9KS_ETXCSR	=	0x30,	/* Early Transmit control/status Reg.*/
  DM9KS_TCCR	=	0x31,	/* Checksum cntrol Reg. */
  DM9KS_RCSR	=	0x32,	/* Receive Checksum status Reg.*/
  DM9KS_MRCMDX	=	0xf0,
  DM9KS_MRCMD	=	0xf2,
  DM9KS_MDRAL	=	0xf4,
  DM9KS_MDRAH	=	0xf5,
  DM9KS_MWCMD	=	0xf8,
  DM9KS_TXPLL	=	0xfc,
  DM9KS_TXPLH	=	0xfd,
  DM9KS_ISR	=	0xfe,
  DM9KS_IMR	=	0xff
};

/* TX status */
#define TX_Jabber_timeout	(1<<7)
#define	TX_LossCarrier		(1<<6)
#define	TX_NoCarrier			(1<<5)
#define	TX_LateColli			(1<<4)
#define TX_ColliPkt				(1<<3)
#define TX_ExcessColli		(1<<2)

/* RX status */
#define RX_RuntFrame				(1<<7)
#define	RX_MultiFrame				(1<<6)
#define	RX_LateColli				(1<<5)
#define	RX_Watchdog_timeout	(1<<4)
#define	RX_PhyErr						(1<<3)
#define	RX_AlignErr					(1<<2)
#define	RX_CRCErr						(1<<1)
#define	RX_FIFO_over				1

/* DM9KS_NCR */
#define NCR_MAC_loopback	2
#define NCR_Reset					1

/* DM9KS_NSR */
#define NSR_10M					(1<<7)
#define NSR_Link_OK			(1<<6)
#define NSR_TX2END			(1<<3)
#define	NSR_TX1END			(1<<2)

/* DM9KS_TCR */
#define TCR_TX_Request	1

/* DM9KS_RXCR */
#define	RXCR_Discard_LongPkt	(1<<5)
#define RXCR_Discard_CRCPkt		(1<<4)
#define	RXCR_Pass_AllMulti		(1<<3)
#define	RXCR_Pass_RuntPkt			(1<<2)
#define	RXCR_Promiscuous			(1<<1)
#define	RXCR_RxEnable			1

/* DM9KS_BPTR */
enum Jam_Pattern_Time{
        JPT_5us = 0,
        JPT_10us = 1,
        JPT_15us = 2,
        JPT_25us = 3,
        JPT_50us = 4,
        JPT_100us = 5,
        JPT_150us = 6,
        JPT_200us = 7,
        JPT_250us = 8,
        JPT_300us = 9,
        JPT_350us = 10,
        JPT_400us = 11,
        JPT_450us = 12,
        JPT_500us = 13,
        JPT_550us = 14,
        JPT_600us = 15
};

/* DM9KS_FCR */
#define	FCR_TX_PausePkt		(1<<5)
#define	FCR_FlowCtlEable	1

/* DM9KS_EPCR */
#define	EPCR_WriteEEPROM_Enable	(1<<5)
#define	EPCR_PHY_Sele						(1<<3) /*bit 3 = 0, select EEPROM*/
#define EPCR_Read								(1<<2)
#define EPCR_Write							(1<<1)

/* DM9KS_EPAR */
#define DM9KS_PHY		0x40	/* PHY address 0x01 */

/* DM9KS_GPR */
#define GPR_PHYDown		1
#define GPR_PHYUp			0

/* DM9KS_TCR2 */
#define TCR2_LedMode1		(1<<7)

/* DM9KS_ETXCSR */
#define ETXCSR_EarlyTrans	(1<<7)
#define	Threshold_12		0
#define	Threshold_25		1
#define	Threshold_50		2
#define	Threshold_75		3

/* DM9KS_TCCR */
#define TCCR_UDP_Chksum 	(1<<2)
#define TCCR_TCP_Chksum		(1<<1)
#define TCCR_IP_Chksum		1

/* DM9KS_RCSR */
#define UDP_Chksum_Err				(1<<7)
#define TCP_Chksum_Err				(1<<6)
#define IP_Chksum_Err					(1<<5)
#define RCSR_RX_Chksum_enable	(1<<1)

/* DM9KS_ISR */
#define ISR_Link_change				(1<<5)
#define ISR_TX_underrun				(1<<4)
#define ISR_RX_OFcnt_overflow	(1<<3)
#define ISR_RX_Overflow				(1<<2)
#define ISR_TX_complete				(1<<1)
#define ISR_RX_coming					1

/* DM9KS_IMR */
#define	IMR_SRAM_antoReturn		(1<<7)
#define IMR_Link_change				(1<<5)
#define IMR_TX_underrun				(1<<4)
#define IMR_RX_OFcnt_overflow	(1<<3)
#define IMR_RX_Overflow				(1<<2)
#define IMR_TX_complete				(1<<1)
#define IMR_RX_coming					1


