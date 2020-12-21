#include "basic_classes.h"
#include <algorithm>
#include <tuple>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include "decode.h"

#define MAX_MASK -1

using namespace std;

// constructor 1
IP::IP(const string& raw_ip) {
    std::vector<string> vec = split(raw_ip, '/');
    std::vector<string>::iterator it = vec.begin();
    
    string raw_ip1 = *it;
    const u_int raw_mask1 = (const u_int)std::stoi(*(++it));
    std::vector<std::string> vec1 = split(raw_ip1, '.');
    std::vector<std::string>::iterator it1 = vec1.begin();
    this->ip = 0;
    this->mask = 0;
    for (; it1 != vec1.end(); it1++) {
        uint8_t t = (uint8_t)std::stoi(*it1);
        this->ip = (this->ip << 8) + t;
    }
    
    if (raw_mask1==0) this->mask = 0;
    else this->mask = ( MAX_MASK << (32 - raw_mask1));
    return;
}
// constructor 2
IP::IP(const string& raw_ip, int raw_mask) {
    std::vector<std::string> vec = split(raw_ip, '.');
    std::vector<std::string>::iterator it = vec.begin();
    this->ip = 0;
    this->mask = 0;
    for (; it != vec.end(); it++) {
        uint8_t t = (uint8_t)std::stoi(*it);
        this->ip = (this->ip << 8) + t;
    }
    // MAX_MASK included in stdint.h
    if (raw_mask==0) this->mask = 0;
    else this->mask = ( MAX_MASK << (32 - raw_mask));
    return;
}
// constructor 3, mask should be 0~32
IP::IP(int ip, int raw_mask) {
    this->ip = ip;
    if (raw_mask==0) this->mask = 0;
    else this->mask = ( MAX_MASK << (32 - raw_mask));
    return;
}


char* IP::showAddr(){
    struct in_addr ip_addr;
    ip_addr.s_addr = htonl(this->ip);
    return inet_ntoa(ip_addr);
}

bool IP::operator<=(const IP& other) {
    //printf ("%d %d %d %d\n", this->ip, this->mask, other.ip, other.mask);
    if ((other.mask <= this->mask) && ((other.mask & other.ip) == (other.mask & this->ip))) {
        return true;
    }
    else {
        return false;
    }
}
/*for type IP, two IPs are equivalent only if they share them same ip, as well as mask*/
bool IP::operator==(const IP& other)const{
    return ((*this).ip == other.ip) && ((*this).mask == other.mask);
}
// reverse result of <=
bool IP::operator!=(const IP& other) {
    if ((other.mask <= this->mask) && ((other.mask & other.ip) == (other.mask & this->ip))) {
        return false;
    }
    else {
        return true;
    }
}

Flow::Flow(int* tag){
    this->headers[TAG] = tag;
    this->headers[SIP] = new IP(0, 0);
    this->headers[DIP] = new IP(0, 0);
    this->headers[IPLEN] = new int(0);
    
}

//return IP/ int
void* & Flow::operator[]  (const string &field) {
    unordered_map<string, void *>::iterator it = field_value.find(field);
    if (it == field_value.end()) {
        ERROR_HANDLE("field "+field+" not in flow, now create a new entry, its tag is "+ to_string(*((int*)this->field_value["tag"])));
        //void * q = new string("error");
        Flow::field_value[field] = q;
        return q;
    }
    return it->second;
}

int Flow::matches(const string &field, const void * p) {
    if ((*this)[field] != NULL) {
        
    }
    return 1;
}
Flow::Flow(u_char * packet, int totallength)  {
    this->pkt = packet;
    int ethernet_header_length = 14;

    EtherHdr* e_hdr = (EtherHdr*) packet;
    if ( ntohs(e_hdr->ether_type) == 0x8100)	
	    ethernet_header_length = 14+4; 
    else
        ethernet_header_length = 14;
    IPHdr * ip_hdr = (IPHdr*) (packet+ethernet_header_length);
    int src_addr = ntohl(ip_hdr->ip_src.s_addr);
	this->headers[SIP] = new IP(src_addr, 32);
    int dst_addr = ntohl(ip_hdr->ip_dst.s_addr);
	this->headers[SIP] = new IP(dst_addr, 32);
}
void Flow::clean() {	
    u_char * packet = this->pkt;
    int ethernet_header_length = 14; 
    EtherHdr* e_hdr = (EtherHdr*) this->pkt;
    if ( ntohs(e_hdr->ether_type) == 0x8100)	
	    ethernet_header_length = 14+4; 
    else
        ethernet_header_length = 14;
    IPHdr * ip_hdr = (IPHdr*) (packet+ethernet_header_length);
    ip_hdr->ip_dst.s_addr = htonl( ((IP *)this->headers[DIP])->ip );
}
