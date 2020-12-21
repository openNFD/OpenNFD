#define FUNCTION rate_limiter
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
#define NUMOFSTREAM 8

using namespace std;

long _counter,_sum = 0;
clock_t begin_t;
clock_t end_t;
u_char *packet;
pcap_t *phandle;
pcap_t *phandle_eth2;
int _allTput=0;
unordered_map<string,int> F_Type::MAP = unordered_map<string,int>();
int process(Flow &f);
Flow f_glb;
struct itimerval tick;
sigset_t newmask;

struct timeval begin_time;
struct timeval end_time;
u_char* packet_set[1000000];
int total_len[1000000];
long int pktindex = 0;

void decodePkt(const u_char *pkt_data)
{
    printf("    Mac : %2x:%2x:%2x:%2x:%2x:%2x",pkt_data[6],pkt_data[7],pkt_data[8],pkt_data[9],pkt_data[10],pkt_data[11]);
    printf(" -> %2x:%2x:%2x:%2x:%2x:%2x\n",pkt_data[0],pkt_data[1],pkt_data[2],pkt_data[3],pkt_data[4],pkt_data[5]);
    printf("    IP  : %3d.%3d.%3d.%3d",pkt_data[26],pkt_data[27],pkt_data[28],pkt_data[29]);
    printf("   -> %3d.%3d.%3d.%3d\n",pkt_data[30],pkt_data[31],pkt_data[32],pkt_data[33]);
}


void stop(int signo)
{
    end_t = clock();
    double total;
    total = (double)(end_t - begin_t)/CLOCKS_PER_SEC;
    printf("\n\n**************************************************\n");
    printf("%ld are forwarded ! Average Tput is %f \n",_counter, _counter / total);
    printf("counter = %ld  sum = %ld  begin = %ld  end = %ld \n", _counter, _sum, begin_t ,end_t);
    printf("**************************************************\n\n");
    _exit(0);
}

/* !!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* INSERTION STARTS FROM HERE*/
Flow f_sig1(new int(1));
void handler(int sig){
    if (sig==SIGALRM){
        process(f_sig1);
    }
}

void _init_(){
    (new F_Type())->init();

    sigset_t newmask;
    sigaddset(&newmask, SIGALRM);
    signal(SIGALRM,handler);
    memset(&tick, 0, sizeof(tick));
    tick.it_value.tv_sec = 0;
    tick.it_value.tv_usec = 0;
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 0;
}
int _t1=100;
int _t2=1;
int _t3=10000;
int _t4=0;
int _t5=0;
State<int> token(_t1);
int process(Flow &f){

if(sigprocmask(SIG_BLOCK,&newmask,NULL)==-1) std::cout<<"block failed"<<endl;
if ((*(int*) f["tag"]) == _t2){
tick.it_value.tv_sec = 1;
tick.it_value.tv_usec =5000;
setitimer(ITIMER_REAL, &tick, NULL);
token[f]=_t3;
}
else if (((*(int*) f["tag"]) == _t4)&&(token[f] < (*(int*) f["iplen"]))){
return -1;
}
else if (((*(int*) f["tag"]) == _t5)&&(token[f] >= (*(int*) f["iplen"]))){
token[f]=token[f] - (*(int*) f["iplen"]);
}
if (sigprocmask(SIG_UNBLOCK, &newmask,NULL)==-1) std::cout<<"unblock failed"<<endl;
        f.clean();
        return 0;
}
int rate_limiter(u_char * pkt,int totallength) {
    f_glb= Flow(pkt,totallength);
    return process(f_glb);
}

int read_local_pcap(int argc, char* argv[]){

    char filePath[100];
    const u_char *packets;
    struct pcap_pkthdr hdr;
    unsigned packetSize;
    pcap_t *descr;

    sprintf(filePath,"/home/hhy/Desktop/univ1_trace/univ1_pt%s.pcap",argv[1]);
        //printf("path : %s\n", filePath[loop]);

    char errbuf[PCAP_ERRBUF_SIZE];
    // pcap_t *fp;
    // if ((fp = pcap_open_live("enp0s31f6",65535,0,1000,errbuf)) == NULL)
    // {
    //     fprintf(stderr,"Unable to open the adapter. %s is not supported. \n","eth0");
    //     return -1;
    // }
    //printf("open the output device success !\n");

        if ((descr = pcap_open_offline(filePath,errbuf)) == NULL)
        {
            printf("error !\n");
            return -1;
        }
        else
        {
            packets = pcap_next(descr,&hdr);
            while (packets!=NULL){
                packetSize = hdr.caplen;
                total_len[pktindex] = packetSize;
                packet_set[pktindex] = new u_char[MAX_PACKET_LEN];
                memcpy(packet_set[pktindex],packets,packetSize);
                pktindex++;
                packets=pcap_next(descr,&hdr);
                //decodePkt(packets[loop]);
            }
            printf("%l",pktindex);
        }

    return 0;

}


int main(int argc, char* argv[])
{
    _init_();
    long int _s =0;
    long int _drop=0;

    if(read_local_pcap(argc,argv)==-1)
    {
        printf("read pcap wrong!\n");
        return 0;
    }

    printf("Packet NUM: %ld\n",pktindex );
    process(f_sig1);
    
    gettimeofday(&begin_time,NULL);
    int _times=1;

    printf("Send times: %d\n", _times);
    for (int j =0;j<_times;j++){
        int i=0;
    for(;i<pktindex;i++){
        if (FUNCTION(packet_set[i],total_len[i])==-1){
            _drop++;
        }

    }}
    _s= _times * pktindex;
    gettimeofday(&end_time,NULL);

    double total = end_time.tv_sec-begin_time.tv_sec + (end_time.tv_usec-begin_time.tv_usec)/(1000000.0);
    printf("    time = %f        pps = %f     sum = %ld    drop = %ld    drop_rate = %f \n",total, _s / total, _s, _drop, (double)_drop/(double)_s);
    FILE *fp = fopen("../Data/LB_Blacklist/LB_blacklist_data.txt","a");
    fprintf(fp,"    time = %f        pps = %f     sum = %ld    drop = %ld    drop_rate = %f \n",total, _s / total, _s, _drop, (double)_drop/(double)_s);
    fclose(fp);

    return 0;
}
