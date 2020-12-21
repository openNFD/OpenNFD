#include "Enclave.h"
#include "Enclave_t.h"  /* print_string */

#include "basic_classes.h"
#include <string.h>


State<struct IP_Port_pair> in_out_map("192.168.1.0/24", 5566);
//to add the out flow


char* baseIP = "192.168.1.0/24";
int selectIP = 2; 
int port = 1;
int i =0;


int NAPT(unsigned char* pkt, uint32_t len) {
    Flow f(pkt);
    if (1){
        // to do : reset the packet 
        i++;
        //printf("%d %d\n", i, f.dport);
        if(strcmp( in_out_map[f].inetIP, "192.168.1.0/24") == 0){
            //printf("11\n");
            struct IP_Port_pair tmp;
            tmp.port = port;
            // strncpy(tmp.inetIP, "192.168.1.",10); 
            snprintf(tmp.inetIP, 20,"%s%d%s", "192.168.1.",selectIP, "/24");
            // printf(" tmp.inetIP: %s\n", tmp.inetIP);
            in_out_map[f] = tmp;
            selectIP ++;
            selectIP = selectIP % 255;

        }   
        else{
            //printf("22\n");

        }
        // packet_counter[f] = packet_counter[f] + 1;

        //printf("sip=%x dip=%x key=%d counter=%d\n", f.sip, f.dip, packet_counter.get_key(f), packet_counter[f]);
    }
    //printf("%d\n", f.dip);

    // if (~(f.dip == 0x0)) {
    //     //printf("c\n");
    //     return -1;
    // }
    //output
    //printf("b\n");
    f.clean();
    return 0;
}

