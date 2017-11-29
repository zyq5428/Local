#include "string.h"
#include "arp.h"

unsigned char sendbuf[1024];

unsigned short checksum(unsigned char * ptr, int len)
{
    unsigned long sum = 0;
    unsigned short *p = (unsigned short *)ptr;

    while (len > 1)
    {
        sum += *p++;
        len -= 2;
    }
    
    if(len == 1)
        sum += *(unsigned char *)p;
    
    while(sum>>16)
        sum = (sum&0xffff) + (sum>>16);
    
    return (unsigned short)((~sum)&0xffff);
}

void tftp_send_request(const char *filename)
{
	unsigned char *ptftp = sendbuf[200];
	unsigned long tftp_len = 0;
	UDP_HDR * udphdr;
	unsigned char * iphdr;
	
	/*Constructs a TFTP packet*/

	/*option code: 1=RRQ;2=WRQ*/
	ptftp[0] = 0x00;
	ptftp[1] = 0x01;
	tftp_len += 2;

	/*file name*/
	sprintf(&ptftp[tftp_len], "%s", filename);
	tftp_len += strlen(filename);
	ptftp[tftp_len] = "0";
	tftp_len += 1;
	
	/*mode*/
	sprintf(&ptftp[tftp_len], "%s", "octect");
	tftp_len += strlen("octect");
	ptftp[tftp_len] = "0";
	tftp_len += 1;
	
	/*Constructs a UDP packet*/
	udphdr = ptftp - sizeof(UDP_HDR);
	iphdr = udphdr + sizeof(ETH_HDR);
	
	udphdr->sport = HON(48915);
	udphdr->dport = HON(69);
	udphdr->len =  HON(tftp_len + sizeof(UDP_HDR) - sizeof(IP_HDR));
	udphdr->udpchecksum = 0x00;
	
	/*Constructs a IP packet*/
	udphdr->iphdr.vhl = 0x45;
	udphdr->iphdr.tos = 0x00;
	udphdr->iphdr.len = HON(tftp_len + sizeof(UDP_HDR) - sizeof(ETH_HDR));
	udphdr->iphdr.ipid = HON(0x00);
	udphdr->iphdr.ipoffset = HON(0x4000);
	udphdr->iphdr.ttl = 0xff;
	udphdr->iphdr.proto = 0x11;
	memcpy(udphdr->iphdr.srcipaddr, ip_addr, 4);
	memcpy(udphdr->iphdr.destipaddr, host_ip_addr, 4);
	udphdr->iphdr.ipchecksum = 0;
	udphdr->iphdr.ipchecksum = checksum(iphdr, 20);
	
	/*Constructs a ETH packet*/
	memcpy(udphdr->iphdr.ethhdr.s_mac, mac_addr, 6);
	memcpy(udphdr->iphdr.ethhdr.d_mac, host_mac_addr, 6);
	udphdr->iphdr.ethhdr.type = HON(PROTO_IP);
	
	 /*call eth_send function */
	eth_send((unsigned long *)udphdr, (sizeof(UDP_HDR) + tftp_len));
}
