#include <stdio.h>
#include <pcap.h>
#include <time.h>
#include "basic_classes.h"
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <cstring>
#include <signal.h>
#include <sys/time.h>

#define RECEIVE_NUM 100000
#define MAX_PACKET_LEN 1500

using namespace std;

long _counter,_sum = 0;

struct timeval begin_time;
struct timeval end_time;

u_char *packet;
pcap_t *phandle;
pcap_t *phandle_eth2;
int _allTput=0;
unordered_map<string,int> F_Type::MAP = unordered_map<string,int>();
unordered_map<string,int> F_Type::MAP2 = unordered_map<string,int>();
int process(Flow &f);
Flow f_glb;
struct itimerval tick;
sigset_t newmask;

void stop(int signo)
{
    gettimeofday(&end_time,NULL);

    double total = end_time.tv_sec-begin_time.tv_sec + (end_time.tv_usec-begin_time.tv_usec)/1000000.0;


    printf("\n\n**************************************************\n");
    printf("%ld are forwarded ! Average Tput is %f \n",_counter, _sum / total);
    printf("counter = %ld  sum = %ld  ", _counter, _sum);
    printf("**************************************************\n\n");
    _exit(0);
}

/* !!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* INSERTION STARTS FROM HERE*/
void _init_(){
    (new F_Type())->init();
}
IP _t1("192.168.0.0/16");
IP _t2("219.168.135.100/32");
int _t3=12000;
#define FUNCTION NAT

int _t4=1;
State<IP> base(_t2);
State<int> port(_t3);
State<unordered_map<IP,unordered_map<int,int>>> mapping(*(new unordered_map<IP,unordered_map<int,int>>()));
State<unordered_map<int,IP>> port_ip(*(new unordered_map<int,IP>()));
State<unordered_map<int,int>> port_port(*(new unordered_map<int,int>()));
int process(Flow &f){

if ((*((IP*) f.headers[SIP])<=_t1)&&(~(mapping[f].find((*(IP*) f.headers[SIP]))!=mapping[f].end()) && (mapping[f].find((*(IP*) f.headers[SIP]))!=mapping[f].end() && ~(mapping[f][(*(IP*) f.headers[SIP])].find((*(int*) f.headers[SPORT]))!=mapping[f][(*(IP*) f.headers[SIP])].end())))){
mapping[f][(*(IP*) f.headers[SIP])][(*(int*) f.headers[SPORT])]=f[f][port[f]];

port_ip[f][port[f]]=(*(IP*) f.headers[SIP]);

port_port[f][port[f]]=f[f][port[f]];

(*(IP*) f.headers[SIP])=base[f];

(*(int*) f.headers[SPORT])=port[f];

port[f]=port[f] + _t4;

}
else if ((*((IP*) f.headers[SIP])<=_t1)&&(mapping[f].find((*(IP*) f.headers[SIP]))!=mapping[f].end() && ~(mapping[f][(*(IP*) f.headers[SIP])].find((*(int*) f.headers[SPORT]))!=mapping[f][(*(IP*) f.headers[SIP])].end()))){
(*(IP*) f.headers[SIP])=base[f];

(*(int*) f.headers[SPORT])=mapping[f][(*(IP*) f.headers[SIP])][(*(int*) f.headers[SPORT])];

}
else if ((*((IP*) f.headers[SIP])!=_t1 && (*(IP*) f.headers[DIP]) == base[f])&&(port_port[f].find((*(int*) f.headers[DPORT]))!=port_port[f].end())){
(*(IP*) f.headers[DIP])=port_ip[f][(*(int*) f.headers[DPORT])];

(*(int*) f.headers[DPORT])=port_port[f][(*(int*) f.headers[DPORT])];

}
else if (*((IP*) f.headers[SIP])!=_t1 && (*(IP*) f.headers[DIP]) != base[f]){
return -1;

}
        f.clean();
        return 0;
}
int NAT(u_char * pkt,int totallength) {
    f_glb= Flow(pkt,totallength);
    return process(f_glb);
}

/* INSERTION ENDS HERE */

void pcap_handle(u_char* user,const struct pcap_pkthdr* header,const u_char* pkt_data)
{
    memcpy(packet,pkt_data,header->caplen);
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!*/
    if (  FUNCTION(packet,header->caplen)!=-1)
    {
        pcap_sendpacket(phandle_eth2,packet,header->len);
        _allTput += header->caplen;
        _counter++;
    }
    _sum++;
}

int main()
{
    _init_();
    char errbuf[1024];
    phandle = pcap_open_live("enp180s0f0",1514,1,0,errbuf);
    //TODO
    phandle_eth2 = pcap_open_live("enp180s0f1",1514,1,0,errbuf);
    if (phandle==NULL){
        cout<<"!!!CAN NOT CAPTURE INTERFACE"<<endl;
        exit(0);
    }
    packet  = new u_char[MAX_PACKET_LEN];
    signal(SIGINT,stop);

    gettimeofday(&begin_time,NULL);
    //process(f_sig1); //FOR Timer Only
    pcap_loop(phandle,-1,pcap_handle,NULL);
    return 0;
}
