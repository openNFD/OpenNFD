#include "Enclave.h"
#include "Enclave_t.h"  /* print_string */
#include "basic_classes.h"


Flow::Flow(unsigned char * packet) {
	this->pkt = packet;
	/* First, lets make sure we have an IP packet */
	struct ether_header *eth_header;
	eth_header = (struct ether_header *) packet;

	/* Pointers to start point of various headers */
	unsigned char *ip_header;
	/* Header lengths in bytes */
	int ethernet_header_length = 14; /* Doesn't change */
	int ip_header_length;
	int tcp_header_length;
	int payload_length;
	/* Find start of IP header */
	ip_header = packet + ethernet_header_length;
	/* The second-half of the first byte in ip_header
	 *        contains the IP header length (IHL). */
	ip_header_length = ((*ip_header) & 0x0F);
	/* The IHL is number of 32-bit segments. Multiply
	 *        by four to get a byte count for pointer arithmetic */
	ip_header_length = ip_header_length * 4;
	/* Show source ip address and destination one */
	int src_addr, des_addr;
	src_addr = *((int *)(ip_header + 12));
	//printf("src ip: %s", inet_ntoa(*address));
	des_addr = *((int *)(ip_header + 16));
	//printf("src ip: %s", inet_ntoa(*address));

    this->sip = src_addr;
	this->dip = des_addr;
}

void Flow::clean() {
	//TODO
}
