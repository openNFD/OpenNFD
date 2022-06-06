/*
 * forwarding.h
 *
 *  Created on: Nov 7, 2017
 *      Author: evan
 */

#ifndef FORWARDING_H_
#define FORWARDING_H_

/*  D A T A  S T R U C T U R E S  *********************************************/


typedef struct _fw_state{
	long packetCount;
}fw_state;
//only record the counter state
typedef struct _globalconfig_{
	fw_state fw_sta;   //the all-flow counter
	char* dev;         //record the device
} GlobalConfig;




/*
 * Ethernet header
 */


//typedef struct _ether_header {
//    uint8_t ether_dst[6];        /* destination MAC */
//    uint8_t ether_src[6];        /* source MAC */
//
//    union {
//        struct etht {
//            uint16_t ether_type; /* ethernet type (normal) */
//        } etht;
//
//        struct qt {
//            uint16_t eth_t_8021; /* ethernet type/802.1Q tag */
//            uint16_t eth_t_8_vid;
//            uint16_t eth_t_8_type;
//        } qt;
//
//        struct qot {
//            uint16_t eth_t_80212;        /* ethernet type/802.1QinQ */
//            uint16_t eth_t_82_mvid;
//            uint16_t eth_t_82_8021;
//            uint16_t eth_t_82_vid;
//            uint16_t eth_t_82_type;
//        } qot;
//    } vlantag;
//
//#define eth_ip_type    vlantag.etht.ether_type
//
//#define eth_8_type     vlantag.qt.eth_t_8021
//#define eth_8_vid      vlantag.qt.eth_t_8_vid
//#define eth_8_ip_type  vlantag.qt.eth_t_8_type
//
//#define eth_82_type    vlantag.qot.eth_t_80212
//#define eth_82_mvid    vlantag.qot.eth_t_82_mvid
//#define eth_82_8021    vlantag.qot.eth_t_82_8021
//#define eth_82_vid     vlantag.qot.eth_t_82_vid
//#define eth_82_ip_type vlantag.qot.eth_t_82_type
//
//} ether_header;

//
//typedef struct _arphdr {
//    uint16_t ar_hrd;            /* Format of hardware address.  */
//    uint16_t ar_pro;            /* Format of protocol address.  */
//    uint8_t ar_hln;             /* Length of hardware address.  */
//    uint8_t ar_pln;             /* Length of protocol address.  */
//    uint16_t ar_op;             /* ARP opcode (command).  */
//#if 0
//    /*
//     * Ethernet looks like this : This bit is variable sized
//     * however...
//     */
//    unsigned char __ar_sha[MAC_ADDR_LEN];       /* Sender hardware address.  */
//    unsigned char __ar_sip[4];  /* Sender IP address.  */
//    unsigned char __ar_tha[MAC_ADDR_LEN];       /* Target hardware address.  */
//    unsigned char __ar_tip[4];  /* Target IP address.  */
//#endif
//} arphdr;
//
//typedef struct _ether_arp {
//    arphdr ea_hdr;              /* fixed-size header */
//    uint8_t arp_sha[MAC_ADDR_LEN];      /* sender hardware address */
//    uint8_t arp_spa[4];         /* sender protocol address */
//    uint8_t arp_tha[MAC_ADDR_LEN];      /* target hardware address */
//    uint8_t arp_tpa[4];         /* target protocol address */
//} ether_arp;
//
//
//
//typedef struct _packetinfo {
//    // macro out the need for some of these
//    // eth_type(pi) is same as pi->eth_type, no?
//    // marked candidates for deletion
//    const struct pcap_pkthdr *pheader; /* Libpcap packet header struct pointer */
//    const uint8_t *  packet;         /* Unsigned char pointer to raw packet */
//    // compute (all) these from packet
//    uint32_t        eth_hlen;       /* Ethernet header lenght */
//    uint16_t        mvlan;          /* Metro vlan tag */
//    uint16_t        vlan;           /* vlan tag */
//    uint16_t        eth_type;       /* Ethernet type (IPv4/IPv6/etc) */
//    uint32_t        af;             /* IP version (4/6) AF_INET */
//    ether_header    *eth_hdr;       /* Ethernet header struct pointer */
//    ether_arp       *arph;          /* ARP header struct pointer */
//    ip4_header      *ip4;           /* IPv4 header struct pointer */
//    ip6_header      *ip6;           /* IPv6 header struct pointer */
//    uint16_t        packet_bytes;   /* Lenght of IP payload in packet */
//    //struct in6_addr ip_src;         /* source address */
//    //struct in6_addr ip_dst;         /* destination address */
//    uint16_t        s_port;         /* source port */
//    uint16_t        d_port;         /* destination port */
//    uint8_t         proto;          /* IP protocoll type */
//    uint8_t         sc;             /* SC_SERVER or SC_CLIENT */
//    tcp_header      *tcph;          /* tcp header struct pointer */
//    udp_header      *udph;          /* udp header struct pointer */
//    icmp_header     *icmph;         /* icmp header struct pointer */
//    icmp6_header    *icmp6h;        /* icmp6 header struct pointer */
//    gre_header      *greh;          /* GRE header struct pointer */
//    uint16_t        gre_hlen;       /* Length of dynamic GRE header length */
//    const uint8_t   *end_ptr;       /* Paranoid end pointer of packet */
//    const uint8_t   *payload;       /* char pointer to transport payload */
//    uint32_t        plen;           /* transport payload length */
//    uint32_t        our;            /* Is the asset in our defined network */
//    uint8_t         up;             /* Set if the asset has been updated */
//    connection      *cxt;           /* pointer to the cxt for this packet */
//    struct _asset    *asset;         /* pointer to the asset for this (src) packet */
//    enum { SIGNATURE, FINGERPRINT } type;
//	bool drop_flag;					/* bit to indicate if packet is to be dropped */
//} packetinfo;
//
//

#endif /* FORWARDING_H_ */


