#include "Enclave.h"
#include "Enclave_t.h"  /* print_string */

#include "basic_classes.h"
#include <stdlib.h>
#include <string.h>


State<char*> lb_table("10.0.1.1/24", 1);
int server_index = 0;

char* IP_set[] = {"10.0.1.10/24", "10.0.1.20/24", "10.0.1.30/24", "10.0.1.40/24", "10.0.1.50/24"}; 


int load_balance(unsigned char* pkt, uint32_t len) {
    Flow f(pkt);
    if (1){

        if(strcmp(lb_table[f], "10.0.1.1/24") == 0){

            int size = strlen(IP_set[server_index]);
            memcpy(lb_table[f], IP_set[server_index], size);
            lb_table[f][size] = '\0';
            server_index = (server_index + 1) % 5;
            // printf("%d\n", server_index);
        }

        //printf("sip=%x dip=%x key=%d server=%s\n", f.sip, f.dip, lb_table.get_key(f), lb_table[f]);
    }

    if (~(f.dip == 0x0)) {
        return -1;
    }
    //output
    f.clean();
    return 0;
}

