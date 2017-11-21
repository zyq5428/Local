#ifndef ARP_H_
#define ARP_H_

typedef struct eth_hdr
{
	unsigned char d_mac[6];
	unsigned char s_mac[6];
	unsigned short type;
}ETH_HDR;

typedef struct arp_hdr
{
	ETH_HDR ethhdr;
	unsigned short hwtype;
	unsigned short protocol;
	unsigned char hwlen;
	unsigned char protolen;
	unsigned short opcode;
	unsigned char smac[6];
	unsigned char sipaddr[4];
	unsigned char dmac[6];
	unsigned char dipaddr[4];
}ARP_HDR;

ARP_HDR arpbuf;

extern unsigned char * buffer;

extern unsigned char host_mac_addr[6];
extern unsigned char mac_addr[6];
extern unsigned char host_ip_addr[4];
extern unsigned char ip_addr[4];
extern unsigned short packet_len;

#endif /* ARP_H_  */

