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
int _t1=100;
#define FUNCTION SYNFD

int _t2=1;
int _t3=1;
int _t4=1;
int _t5=1;
int _t6=1;
int _t7=1;
int _t8=1;
int _t9=1;
int _t10=1;
int _t11=1;
int _t12=1;
int _t13=1;
int _t14=1;
State<unordered_map<IP,int>> blist(*(new unordered_map<IP,int>()));
State<int> threshold(_t1);
int process(Flow &f){

if ((*(int*) f.headers[FLAG_SYN]) == _t2 && (*(int*) f.headers[TAG]) != _t3){
blist[f][(*(IP*) f.headers[SIP])]=blist[f][(*(IP*) f.headers[SIP])] + _t4;

(*(int*) f.headers[TAG])=_t5;

}
else if (((*(int*) f.headers[TAG]) == _t6)&&(blist[f][(*(IP*) f.headers[SIP])] == threshold[f])){
return -1;

}
else if (((*(int*) f.headers[TAG]) == _t7)&&(blist[f][(*(IP*) f.headers[SIP])] != threshold[f])){
}
else if ((*(int*) f.headers[TAG]) != _t8 && (*(int*) f.headers[FLAG_SYN]) != _t9 && (*(int*) f.headers[FLAG_ACK]) == _t10){
blist[f][(*(IP*) f.headers[SIP])]=blist[f][(*(IP*) f.headers[SIP])] - _t11;

}
else if ((*(int*) f.headers[TAG]) != _t12 && (*(int*) f.headers[FLAG_SYN]) != _t13 && (*(int*) f.headers[FLAG_ACK]) != _t14){
}
        f.clean();
        return 0;
}
int SYNFD(u_char * pkt,int totallength) {
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
