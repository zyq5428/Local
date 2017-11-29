#ifndef ARP_H_
#define ARP_H_

#define HON(n) (((n & 0xff) << 8 ) | ((n & 0xff00) >> 8))

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

typedef struct ip_hdr
{
	ETH_HDR ethhdr;
	unsigned char vhl;
	unsigned char tos;
	unsigned short len;
	unsigned short ipid;
	unsigned short ipoffset;
	unsigned char ttl;
	unsigned char proto;
	unsigned short ipchecksum;
	unsigned char srcipaddr[4];
	unsigned char destipaddr[4];
}IP_HDR;

typedef struct udp_hdr
{
	IP_HDR iphdr;
	unsigned short sport;
	unsigned short dport;
	unsigned short len;
	unsigned short udpchecksum;
}UDP_HDR;

ARP_HDR arpbuf;

#define PROTO_ARP (0x0806)
#define PROTO_IP (0x0800)

extern unsigned char host_mac_addr[6];
extern unsigned char mac_addr[6];
extern unsigned char host_ip_addr[4];
extern unsigned char ip_addr[4];
extern unsigned short packet_len;

#endif /* ARP_H_  */

