#include "basic_classes.h"
#include <algorithm>
#include <tuple>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include "decode.h"

using namespace std;

// constructor 1
IP::IP(const string& raw_ip) {
	std::vector<string> vec = split(raw_ip, '/');
	std::vector<string>::iterator it = vec.begin();

	string raw_ip1 = *it;
	int raw_mask1 = std::stoi(*(++it));

	std::vector<std::string> vec1 = split(raw_ip1, '.');
	std::vector<std::string>::iterator it1 = vec1.begin();
	this->ip = 0;
	this->mask = 0;
	for (; it1 != vec1.end(); it1++) {
		uint8_t t = (uint8_t)std::stoi(*it1);
		this->ip = (this->ip << 8) + t;
	}
	// UINT32_MAX included in stdint.h
	this->mask = UINT32_MAX << (32 - raw_mask1);
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
	// UINT32_MAX included in stdint.h
	this->mask = UINT32_MAX << (32 - raw_mask);
	return;
}
// constructor 3, mask should be 0~32
IP::IP(int ip, int raw_mask) {
	this->ip = ip;
	this->mask = UINT32_MAX << (32 - raw_mask);
	return;
}


char* IP::showAddr(){
	struct in_addr ip_addr;
	ip_addr.s_addr = htonl(this->ip);
	return inet_ntoa(ip_addr);
}
/*param:
  return 1(true) if success, 0(false) for failure
  */
/* tips: ip1 contains ip2 == ip2 <= ip1 
   bool IP::contains(const IP& ip2) const
   {
//return ip2 <= (*this);
if ((ip2.mask <= this->mask) && ((ip2.mask & ip2.ip) == (ip2.mask & this->ip))) {
return true;
}
else {
return false;
}
}
bool IP::operator>=(const IP& other) {
return (*this).contains(other);
}
*/
bool IP::operator<=(const IP& other) {
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

/* param:
field: string
value: void *
*/
/*
   Flow::Flow(int count, ...) {
   va_list ap;
   va_start(ap, count);
   for (int i = 0; i < count; i = i + 2) {
   string field = va_arg(ap, string);
   void * value = va_arg(ap, void *);
   Flow::field_value[field] = value;
   }
   va_end(ap);
   }
   Flow::Flow(string str) {
   std::vector<string> vec = split(str, '&');
   std::vector<string>::iterator it = vec.begin();
   for (; it != vec.end(); it++) {
   Flow::field_value[*it] = new void*;
   }
   }*/
Flow::Flow(int* tag){
	this->field_value["tag"] = tag;
	this->field_value["sip"] = new IP(0, 0);
	this->field_value["dip"] = new IP(0, 0);
	this->field_value["iplen"] = new int(0);
}

Flow::Flow(u_char * packet, int totallength)  {
	/*Decoding*/	
	this->pkt = packet;
	struct ether_header *eth_header;
	eth_header = (struct ether_header *) packet;

	/* Pointers to start point of various headers */
	u_char *ip_header;
	/* Header lengths in bytes */
	int ethernet_header_length = 14; /* Doesn't change */
	int ip_header_length;
	ip_header = packet + ethernet_header_length;
	ip_header_length = ((*ip_header) & 0x0F);
	ip_header_length = ip_header_length * 4;
	int src_addr, des_addr;
	src_addr = ntohl(*((int *)(ip_header + 12)));
	des_addr = ntohl(*((int *)(ip_header + 16)));
	this->field_value["sip"] = new IP(src_addr, 32);
	this->field_value["dip"] = new IP(des_addr, 32);
	this->field_value["tag"] = new int(0);
	this->field_value["iplen"] = new int(totallength);
    short protocol = ntohs(*(short *)(ip_header+9));
    this->field_value["UDP"] = new int((protocol==IPPROTO_UDP)?1:0); 

	/*TCP layer*/
	TCPHdr *tcph =(TCPHdr *)(ip_header+ip_header_length);
	this->field_value["sport"]=new int(ntohs(tcph->th_sport));
	this->field_value["dport"] = new int(ntohs(tcph->th_dport));
	/*URG ACK PSH RST SYN FIN*/
	this->field_value["flag_fin"] = new int(ntohs(tcph->th_flags) & TH_FIN);
	this->field_value["flag_syn"] = new int(ntohs(tcph->th_flags) & TH_SYN);
    this->field_value["flag_ack"] = new int(ntohs(tcph->th_flags) & TH_ACK);
    

}
void Flow::clean() {
	/*Encoding*/	
	struct ether_header *eth_header;
	eth_header = (struct ether_header *) this->pkt;
	int ethernet_header_length = 14; /* Doesn't change */
	int ip_header_length;
	u_char *ip_header;
	ip_header = (u_char*)eth_header + ethernet_header_length;
	ip_header_length = ((*ip_header) & 0x0F);
	ip_header_length = ip_header_length * 4;

	*((int *)(ip_header + 12)) = htonl(* ((int *)this->field_value["sip"]));
	*((int *)(ip_header + 16)) = htonl(* ((int *)this->field_value["dip"]));
	//this->field_value["iplen"] = new int(totallength);

	/*TCP layer*/
	TCPHdr *tcph =(TCPHdr *)(ip_header+ip_header_length);
	tcph->th_sport =u_short(*((int*)this->field_value["sport"]));
	tcph->th_dport =u_short(*((int*)this->field_value["dport"]));
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


/*
   FlowSet::FlowSet(int count, ...) {
   va_list ap;
   va_start(ap, count);
   for (int i = 0; i < count; i++) {
   Flow f = va_arg(ap, Flow);
   this->insert(f);
   }
   va_end(ap);
   }
   int FlowSet::contains(const Flow &object) {
   return this->find(object) == this->end();
   }
   FlowSet FlowSet::operator+(Flow flow1) {
   return (*this) + FlowSet(1, flow1);
   }
   inline FlowSet FlowSet::operator+(FlowSet set1) {
   FlowSet nset;
   for (iterator it = this->begin(); it != this->end(); it++) {
   nset.insert(*it);
   }
   for (iterator it = set1.begin(); it != set1.end(); it++) {
   nset.insert(*it);
   }
   return nset;
   }
   bool FlowCmp::operator()(const Flow &a, const Flow &b)const {
//TODO
auto it = a.field_value.begin();
for (; it != a.field_value.end(); it++){

}
return true;
}

bool IPcmp::operator()(const IP &a, const IP &b)const {
return a.ip < b.ip || a.ip == b.ip&& a.mask < b.mask;
}
IPset::IPset(int count, ...) {
va_list ap;
va_start(ap, count);
for (int i = 0; i < count; i++) {
IP ip = va_arg(ap, IP);
this->insert(ip);
}
va_end(ap);
}
int IPset::contains(const IP &object) {
return this->find(object) == this->end();
}
IPset IPset::operator+(IP ip1) {
return (*this) + IPset(1, ip1);
}
IPset IPset::operator+(IPset set1) {
IPset nset;
for (iterator it = this->begin(); it != this->end(); it++) {
nset.insert(*it);
}
for (iterator it = set1.begin(); it != set1.end(); it++) {
nset.insert(*it);
}
return nset;
}
*/
