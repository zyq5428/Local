/*
 * 			U-BOOT DM9000A DRIVER
 *			  www.davicom.com.tw
 * 
 * This program is loaded into SRAM in bootstrap mode, where it waits
 * for commands on UART1 to read and write memory, jump to code etc.
 * A design goal for this program is to be entirely independent of the
 * target board.  Anything with a CL-PS7111 or EP7211 should be able to run
 * this code in bootstrap mode.  All the board specifics can be handled on
 * the host.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *V1.01 -load MAC address from EEPROM
 */

#include <common.h>
#include <command.h>
#include <net.h>

#ifdef CONFIG_DRIVER_DM9000
#include "dm9000x.h"



#define DM9000_ID		0x90000A46
#define DM9010_ID		0x90100A46
#define DM9KS_REG05		(RXCR_Discard_LongPkt|RXCR_Discard_CRCPkt) 
#define DM9KS_DISINTR		IMR_SRAM_antoReturn
/*
 * If your bus is 8-bit or 32-bit, you must modify below.
 * Ex. your bus is 8 bit
 *	DM9000_PPTR *(volatile u8 *)(DM9000_BASE)
 */

#define DM9000_BASE		CONFIG_DM9000_BASE

#define DM9000_PPTR		*(volatile u16 *)(DM9000_IO)
#define DM9000_PDATA		*(volatile u16 *)(DM9000_DATA)


#define mdelay(n)       udelay((n)*1000)

static unsigned char ior(int);
static void iow(int, u8);
static void phy_write(int, u16);
static void move8(unsigned char *, int, int);
static void move16(unsigned char *, int, int);
static void move32(unsigned char *, int, int);
static u16 read_srom_word(int);
static void dmfe_init_dm9000(void);
static u32 GetDM9000ID(void);
void DM9000_get_enetaddr (uchar *);
static void eth_reset (void);
void eth_halt(void);
int eth_init(bd_t *);
void (*MoveData)(unsigned char *, int , int);

static void iow(int reg, u8 value)
{
        DM9000_PPTR = reg;
        DM9000_PDATA  =  value & 0xff;
}

static unsigned char ior(int reg)
{
        DM9000_PPTR = reg;
        return DM9000_PDATA & 0xff;
}

static void phy_write(int reg, u16 value)
{
        /* Fill the phyxcer register into REG_0C */
        iow(DM9KS_EPAR, DM9KS_PHY | reg);

        /* Fill the written data into REG_0D & REG_0E */
        iow(DM9KS_EPDRL, (value & 0xff));
        iow(DM9KS_EPDRH, ( (value >> 8) & 0xff));

        iow(DM9KS_EPCR, EPCR_PHY_Sele|EPCR_Write);	/* Issue phyxcer write command */
        udelay(500);			/* Wait write complete */
        iow(DM9KS_EPCR, 0x0);	/* Clear phyxcer write command */
}

/*
        leng: unit BYTE
        selec 0:input(RX)	1:output(TX)
        if selec=0, move data from FIFO to data_ptr 
        if selec=1, move data from data_ptr to FIFO
*/
static void move8(unsigned char *data_ptr, int leng, int selec)
{
        int i;
        if (selec)
                for (i=0; i<leng; i++)
                        DM9000_PDATA =(data_ptr[i] & 0xff);
        else
                for (i=0; i<leng; i++)
                        data_ptr[i] = DM9000_PDATA;
}	

static void move16(unsigned char *data_ptr, int leng, int selec)
{
        int i, tmpleng;
        tmpleng = (leng + 1) >> 1;
        if (selec)
                for (i=0; i<tmpleng; i++)
                        DM9000_PDATA =((u16 *)data_ptr)[i];
        else
                for (i=0; i<tmpleng; i++)
                        ((u16 *)data_ptr)[i] = DM9000_PDATA;
}	

static void move32(unsigned char *data_ptr, int leng, int selec)
{
        int i, tmpleng;
        tmpleng = (leng + 3) >> 2;
        if (selec)
                for (i=0; i<tmpleng; i++)
                        DM9000_PDATA = ((u32 *)data_ptr)[i];
        else
                for (i=0; i<tmpleng; i++)
                        ((u32 *)data_ptr)[i]=DM9000_PDATA;
}	

/*
 * Read a word data from EEPROM
 */
static u16 read_srom_word(int offset)
{
        iow(DM9KS_EPAR, offset);
        iow(DM9KS_EPCR, 0x4);
        udelay(200);
        iow(DM9KS_EPCR, 0x0);
        return (ior(DM9KS_EPDRL) + (ior(DM9KS_EPDRH) << 8) );
}
/* 
 *	Initilize dm9000 board
 */
static void dmfe_init_dm9000(void)
{
        int io_mode;
        
        /* set the internal PHY power-on, GPIOs normal, and wait 20ms */
        iow(DM9KS_GPR, GPR_PHYUp);
        mdelay(20); /* wait for PHY power-on ready */
        iow(DM9KS_GPR, GPR_PHYDown);/* Power-Down PHY */
        mdelay(1000);	/* compatible with rtl8305s */
        iow(DM9KS_GPR, GPR_PHYUp);	
        mdelay(20);/* wait for PHY power-on ready */

        iow(DM9KS_NCR, NCR_MAC_loopback|NCR_Reset);
        udelay(20);/* wait 20us at least for software reset ok */
        iow(DM9KS_NCR, NCR_MAC_loopback|NCR_Reset);
        udelay(20);/* wait 20us at least for software reset ok */

        /* I/O mode */
        io_mode = ior(DM9KS_ISR) >> 6; /* ISR bit7:6 keeps I/O mode */
        switch (io_mode)
        {
                case DM9KS_BYTE_MODE:
                        printf("DM9000 work in 8 bus width\n");
                        MoveData = move8;
                        break;
                case DM9KS_WORD_MODE:
                        printf("DM9000 work in 16 bus width\n");
                        MoveData = move16;
                        break;
                case DM9KS_DWORD_MODE:
                        printf("DM9000 work in 32 bus width\n");
                        MoveData = move32;
                        break;
                default:
                        printf("DM9000 work in wrong bus width, error\n");
                        break;
        }
        
        /* Set PHY */
        phy_write(4, 0x01e1);
        phy_write(0, 0x1200); /* N-way */

        /* Program operating register */
        iow(DM9KS_NCR, 0);
        iow(DM9KS_TCR, 0);/* TX Polling clear */
        iow(DM9KS_BPTR, 0x30|JPT_600us);/* Less 3kb, 600us */
        iow(DM9KS_SMCR, 0);/* Special Mode */
        iow(DM9KS_NSR, 0x2c);/* clear TX status */
        iow(DM9KS_ISR, 0x0f);/* Clear interrupt status */
        iow(DM9KS_TCR2, TCR2_LedMode1);/* Set LED mode 1 */
#if 0
        /* Data bus current driving/sinking capability  */
        iow(DM9KS_PBCR, 0x60);	/* default: 8mA */
#endif
 
        iow(0x1d, 0x80);/* receive broadcast packet */

        /* Activate DM9000A/DM9010 */
        iow(DM9KS_RXCR, DM9KS_REG05 | RXCR_RxEnable);	
        iow(DM9KS_IMR, DM9KS_DISINTR);
}

/* packet page register access functions */
static u32 GetDM9000ID(void)
{
        u32	id_val;

        DM9000_PPTR = DM9KS_PID_H;
        id_val = (DM9000_PDATA & 0xff) << 8;
        DM9000_PPTR = DM9KS_PID_L;
        id_val+= (DM9000_PDATA & 0xff);
        id_val = id_val << 16;
        
        DM9000_PPTR = DM9KS_VID_H;
        id_val += (DM9000_PDATA & 0xff) << 8;
        DM9000_PPTR = DM9KS_VID_L;
        id_val += (DM9000_PDATA & 0xff);
        

        return id_val;
}


void DM9000_get_enetaddr (uchar * addr)
{
        int i;
        u8 temp;
        eth_reset ();	
        printf ("MAC: ");
        for (i = 0x10; i <= 0x15; i++) {
                temp = ior (i);
                *addr++ = temp;
                printf ("%x:", temp);
        }

        return;
}

static void eth_reset (void)
{
        u32 ID;
        
        ID = GetDM9000ID();
        if ( ID != DM9000_ID)
        {
                printf("not found the dm9000 ID:%x\n",ID);
                return ;
        }else
                printf("found DM9000 ID:%x\n",ID);
        eth_halt();
        dmfe_init_dm9000();
}

void eth_halt (void)
{
        /* RESET devie */
        phy_write(0x00, 0x8000);	/* PHY RESET */
        iow(DM9KS_GPR, GPR_PHYDown); 	/* Power-Down PHY */
        iow(DM9KS_IMR, DM9KS_DISINTR);	/* Disable all interrupt */
        iow(DM9KS_RXCR, 0x00);		/* Disable RX */
}

int eth_init (bd_t * bd)
{
        u32 ID;
        int i,j;
        u16 * mac =(u16 *) bd->bi_enetaddr;

        ID = GetDM9000ID();
        if ( ID != DM9000_ID)
        {
                printf("not found the dm9000 ID:%x\n",ID);
                return 1;
        }
        printf("Found DM9000 ID:%x at address %x !\n", ID,  DM9000_BASE);
        dmfe_init_dm9000();

        {
          int env_size;
          char *s = NULL, *e = NULL;
          unsigned char env_mac[20];
          env_size = getenv_r("ethaddr", env_mac, sizeof(env_mac));
          if(env_size != 18)
            {
              printf("\n***ERROR: ethaddr is not set properly!!\n");
#if 0
              for (i=0; i<3; i++) /* read MAC from EEPROM */
                mac[i]= read_srom_word(i);
#else
              mac[0]=0x1100;
              mac[1]=0x3322;
              mac[2]=0x5544;
#endif
            }
          else 
            {
              s = env_mac;
              for(i = 0; i < 6; ++i)
                {
                  (bd->bi_enetaddr)[i] = s ? simple_strtoul(s, &e, 16) : 0;
                  if(s)  s = (*e) ? e + 1 : e;
                }
              printf("bd->bi_entaddr: %02x:%02x:%02x:%02x:%02x:%02x\n", bd->bi_enetaddr[0],
                     bd->bi_enetaddr[1], bd->bi_enetaddr[2], bd->bi_enetaddr[3],
                     bd->bi_enetaddr[4], bd->bi_enetaddr[5]);
            }
        }
        printf("[eth_init]MAC:");
        for(i=0,j=0x10; i<6; i++,j++)
          {	
            iow(j,bd->bi_enetaddr[i]);
            printf("%x:",bd->bi_enetaddr[i]);
          }
        printf("\n");

        return 0;
}

/* Get a data block via Ethernet */
extern int eth_rx (void)
{
        unsigned short rxlen;
        unsigned char *addr = NULL;
        u8 RxRead;
        rx_t rx;
        u8 * ptr = (u8*)&rx;
        

        RxRead = ior(DM9KS_MRCMDX);
        RxRead = ior(DM9KS_ISR);
        RxRead = ior(DM9KS_MRCMDX) & 0xff;

        if (RxRead != 1)  /* no data */ 
                return 0;

        DM9000_PPTR = DM9KS_MRCMD; /* set read ptr ++ */

        /* Read packet status & length */
        MoveData(ptr, 4, 0);

        rxlen = rx.desc.length;		/* get len */

        if(rx.desc.status & (RX_RuntFrame | RX_PhyErr | RX_AlignErr | RX_CRCErr))
                printf ("[dm9ks]RX error %x\n", rx.desc.status);	

        if (rxlen > PKTSIZE_ALIGN + PKTALIGN)
                printf ("packet too big! %d %d\n", rxlen, PKTSIZE_ALIGN + PKTALIGN);

        addr = (unsigned char *)NetRxPackets[0];
        MoveData(addr, rxlen, 0);

        /* Pass the packet up to the protocol layers. */
        NetReceive (NetRxPackets[0], rxlen);

        return rxlen;
}

/* Send a data block via Ethernet. */
extern int eth_send (volatile void *packet, int length)
{
        volatile unsigned char *addr;
        int	length1 = length;

        DM9000_PPTR = DM9KS_MWCMD;/* data copy ready set */

        /* copy data */
        addr = packet;
        MoveData(addr,length,1);

        /* set packet length  */
        iow(DM9KS_TXPLH, (length1 >> 8) & 0xff);  
        iow(DM9KS_TXPLL, length1 & 0xff);

        /* start transmit */
        iow(DM9KS_TCR, TCR_TX_Request);

        while (1)/* wait for tx complete */
        {
                if (ior(DM9KS_NSR)& (NSR_TX2END|NSR_TX1END))	
                        break;
        }
        return 0;
}

#endif				/* CONFIG_DRIVER_DM9000 */



