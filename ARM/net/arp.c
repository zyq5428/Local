#include "arp.h"

/*
 * send arp request packet
 */
 
int ARP_FLAG;
 
 void arp_request(void)
 {
	printf("\n\r arp request \n\r");
	
	 /*Constructs an arp request packet */
	memcpy(arpbuf.ethhdr.d_mac, host_mac_addr, 6);
	memcpy(arpbuf.ethhdr.s_mac, mac_addr, 6);
	arpbuf.ethhdr.type = HON(0x0806);
	
	arpbuf.hwtype = HON(1);
	arpbuf.protocol = HON(0x0800);
	arpbuf.hwlen = 6;
	arpbuf.protolen = 4;
	arpbuf.opcode = HON(1);
	memcpy(arpbuf.smac, mac_addr, 6);
	memcpy(arpbuf.sipaddr, ip_addr, 4);
	//arpbuf.dmac[6]
	memcpy(arpbuf.dipaddr, host_ip_addr, 4);
	
	packet_len = 14 + 28;

	 /*call eth_send function */
	printf("\n\r call eth_send function \n\r");
	eth_send(&arpbuf, packet_len);
	
	ARP_FLAG = 1;
	printf("\n\r ARP_FLAG is : %d\n\r", ARP_FLAG);
	
	printf("\n\r arp request end \n\r");
 }


 /*
  * Parse arp response packet, extract MAC
  */
unsigned char arp_process(unsigned char * buf, unsigned long len)
{
	int i;
	ARP_HDR * arp_p = (ARP_HDR *)buf;
	
	printf("\n\r arp process \n\r");
	
	if (packet_len < 28)
		return 0;
	
	switch (HON(arp_p->opcode)) {
		case 2: /*ARP Request*/
		if (host_ip_addr[3] == arp_p->sipaddr[3]) {
			ARP_FLAG = 0;
			printf("\n\r ARP_FLAG is : %d\n\r", ARP_FLAG);
		}
		memcpy(host_ip_addr, arp_p->sipaddr, 4);
		printf("\n\r host ip is : ");
		for (i = 0; i < 4; i++) {
			printf("%03d", host_ip_addr[i]);
			printf(".");
			}
		printf("\n\r");
	
		memcpy(host_mac_addr, arp_p->smac, 6);
		printf("\n\r host mac is : ");
		for (i = 0; i < 6; i++) {
			printf("%02x", host_mac_addr[i]);
			printf("-");
			}
		printf("\n\r");
	
		break;
		
		case 1: /*ARP Acknowledge*/
		/*send arp reauest packet*/
		/*Constructs an arp request packet */
		memcpy(arpbuf.ethhdr.d_mac, arp_p->smac, 6);
		memcpy(arpbuf.ethhdr.s_mac, mac_addr, 6);
		arpbuf.ethhdr.type = HON(0x0806);
	
		arpbuf.hwtype = HON(1);
		arpbuf.protocol = HON(0x0800);
		arpbuf.hwlen = 6;
		arpbuf.protolen = 4;
		arpbuf.opcode = HON(2);
		memcpy(arpbuf.smac, mac_addr, 6);
		memcpy(arpbuf.sipaddr, ip_addr, 4);
		memcpy(arpbuf.dmac, arp_p->smac, 6);
		memcpy(arpbuf.dipaddr, arp_p->sipaddr, 4);
	
		packet_len = 14 + 28;

		/*call eth_send function */
		eth_send(&arpbuf, packet_len);
		
		printf("\n\r arp process end \n\r");
		
		break;
	}
}

int get_arp_flag(void)
{
	return ARP_FLAG;
}

void ip_process(unsigned char * buf, unsigned long len)
{
	
}

void net_handle(unsigned char * buf, unsigned long len)
{
	ETH_HDR * p = (ETH_HDR *)buf;
	
	//printf("\n\r net handle \n\r");

	switch (HON(p->type)) {
		case PROTO_ARP:
			arp_process(buf, len);
			break;

		case PROTO_IP:
			ip_process(buf, len);
			break;

		default:
			break;
	}
	
	//printf("\n\r net handle end \n\r");
}

/*void dm9000_arp(void)
{
	ARP_FLAG = 1;
	arp_request();	
	//while (ARP_FLAG) ;
}*/
