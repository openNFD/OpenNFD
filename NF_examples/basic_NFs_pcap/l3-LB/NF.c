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
int _t1=0;
IP _t2("10.0.1.10/32");
int _t3=1;
IP _t4("10.0.1.20/32");
int _t5=2;
IP _t6("10.0.1.30/32");
int _t7=3;
IP _t8("10.0.1.40/32");
int _t9=4;
IP _t10("10.0.1.50/32");
int _t11=0;
int _t12=5;
#define FUNCTION L3_LB

int _t13=1;
State<unordered_map<int,IP>> list(create_map<int,IP>(*(new unordered_map<int,IP>()),10,&(_t1),&(_t2),&(_t3),&(_t4),&(_t5),&(_t6),&(_t7),&(_t8),&(_t9),&(_t10)));
State<int> round(_t11);
State<int> round_max(_t12);
int process(Flow &f){

if (1){
(*(IP*) f.headers[DIP])=list[f][round[f]];

round[f]=(round[f] + _t13) % round_max[f];

}
        f.clean();
        return 0;
}
int L3_LB(u_char * pkt,int totallength) {
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
