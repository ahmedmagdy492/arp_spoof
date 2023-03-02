#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <linux/if.h>
#include <linux/types.h>
#include <sys/ioctl.h>

#include "nt_headers.h"
#include "util.h"
#include "arg_parser.h"

#define ETH_P_ARP 0x0806

int main(int argc, char* argv[]) {

	char interface[24];
	char target_ip[16];

	int result = parse_args(argv, argc, interface, target_ip);

	if(!result) {
		exit(-1);
	}

	int sock = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
	if(sock < 0) {
		perror("while creating raw socket");
		exit(-1);
	}

	// getting the index of the interface to send the packets on
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));

	strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
	
	if((ioctl(sock, SIOCGIFINDEX, &ifr)) < 0) {
		perror("while getting the interface index");
		exit(-1);
	}

	// getting the mac address of the current interface
	struct ifreq ifr_macAddr;
	memset(&ifr_macAddr, 0, sizeof(ifr_macAddr));
	strncpy(ifr_macAddr.ifr_name, interface, IFNAMSIZ-1);

	if((ioctl(sock, SIOCGIFHWADDR, &ifr_macAddr)) < 0) {
		perror("while getting the mac address of the current interface");
		exit(-1);
	}

	// getting the ip of the interface
	struct ifreq ifr_ip;
	memset(&ifr_ip, 0, sizeof(ifr_ip));
	strncpy(ifr_ip.ifr_name, interface, IFNAMSIZ-1);

	if((ioctl(sock, SIOCGIFADDR, &ifr_ip)) < 0) {
		perror("while getting the ip of the interface");
		exit(-1);
	}

	char* send_buffer = (char*)malloc(sizeof(char)*64);
	memset(send_buffer, 0, 64);

	int total_len = 0;
	
	struct ethhdr *eth = (struct ethhdr *)(send_buffer);

	// filling up my source mac address
	eth->h_source[0] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[0]);
	eth->h_source[1] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[1]);
	eth->h_source[2] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[2]);
	eth->h_source[3] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[3]);
	eth->h_source[4] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[4]);
	eth->h_source[5] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[5]);

	// filling up dest mac address
	eth->h_dest[0] = 0xf4;
	eth->h_dest[1] = 0x7b;
	eth->h_dest[2] = 0x09;
	eth->h_dest[3] = 0x12;
	eth->h_dest[4] = 0x53;
	eth->h_dest[5] = 0xfe;
	eth->h_proto = htons(ETH_P_ARP);
	total_len += sizeof(struct ethhdr);

	// // creating the arp header
	struct arphdr *arp = (struct arphdr*)(send_buffer+total_len);
	
	arp->hd_type[0] = 0x0;
	arp->hd_type[1] = 0x1;
	arp->ip_v[0] = 0x8;
	arp->ip_v[1] = 0x0;
	arp->hd_s = 0x6;
	arp->ip_s = 0x4;
	arp->op_code[0] = 0x0;
	arp->op_code[1] = 0x2;
	arp->src_mac[0] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[0]);
	arp->src_mac[1] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[1]);
	arp->src_mac[2] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[2]);
	arp->src_mac[3] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[3]);
	arp->src_mac[4] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[4]);
	arp->src_mac[5] = (unsigned char)(ifr_macAddr.ifr_hwaddr.sa_data[5]);

	char* src_ip = inet_ntoa((((struct sockaddr_in *)&(ifr_ip.ifr_addr))->sin_addr));

	int ip[4];
	extract_ip(src_ip, ip);

	arp->src_ip[0] = (unsigned char)ip[0];
	arp->src_ip[1] = (unsigned char)ip[1];
	arp->src_ip[2] = (unsigned char)ip[2];
	arp->src_ip[3] = (unsigned char)ip[3];

	arp->dst_mac[0] = 0xd4;
	arp->dst_mac[1] = 0x6b;
	arp->dst_mac[2] = 0xa6;
	arp->dst_mac[3] = 0xdf;
	arp->dst_mac[4] = 0xe6;
	arp->dst_mac[5] = 0xab;

	int dst_ip[4];
	extract_ip(target_ip, dst_ip);

	arp->dst_ip[0] = dst_ip[0];
	arp->dst_ip[1] = dst_ip[1];
	arp->dst_ip[2] = dst_ip[2];
	arp->dst_ip[3] = dst_ip[3];

	total_len += sizeof(struct arphdr);

	struct sockaddr_ll sadr_ll;
	sadr_ll.sll_ifindex = ifr.ifr_ifindex; // index of interface
	sadr_ll.sll_halen = ETH_ALEN; // length of destination mac address
	sadr_ll.sll_addr[0] = 0xf4;
	sadr_ll.sll_addr[1] = 0x7b;
	sadr_ll.sll_addr[2] = 0x09;
	sadr_ll.sll_addr[3] = 0x12;
	sadr_ll.sll_addr[4] = 0x53;
	sadr_ll.sll_addr[5] = 0xfe;

	int send_len = sendto(sock,send_buffer,64,0,(const struct sockaddr*)&sadr_ll,sizeof(struct sockaddr_ll));
	if(send_len<0)
	{
		perror("while sending the packet");
		exit(-1);
	}

	printf("Sent ARP Replay Packet\n");

	close(sock);
}
