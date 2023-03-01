#define ETH_ALEN 6
#define IP_LEN 4

struct ethhdr {
	unsigned char h_dest[ETH_ALEN];
	unsigned char h_source[ETH_ALEN];
	unsigned short h_proto;
};

struct arphdr {
	unsigned char hd_type[2];
	unsigned char ip_v[2];
	unsigned char hd_s;
	unsigned char ip_s;
	unsigned char op_code[2];
	unsigned char src_mac[ETH_ALEN];
	unsigned char src_ip[IP_LEN];
	unsigned char dst_mac[ETH_ALEN];
	unsigned char dst_ip[IP_LEN];
};

struct sockaddr_ll {
	unsigned short sll_family;   /* Always AF_PACKET */
	unsigned short sll_protocol; /* Physical-layer protocol */
	int            sll_ifindex;  /* Interface number */
	unsigned short sll_hatype;   /* ARP hardware type */
	unsigned char  sll_pkttype;  /* Packet type */
	unsigned char  sll_halen;    /* Length of address */
	unsigned char  sll_addr[8];  /* Physical-layer address */
};