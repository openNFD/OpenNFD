/*
 * Copyright (C) 2011-2018 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>

# include <unistd.h>
# include <pwd.h>
# define MAX_PATH FILENAME_MAX

#include "sgx_urts.h"
#include "App.h"
#include "Enclave_u.h"

//counter
#include <time.h>
#include <pcap.h>
#include <sys/wait.h>
u_char *packet;
pcap_t *phandle;
long counter = 0;
clock_t begin_t;
clock_t end_t;
#define MAX_PACKET_LEN 1500

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;


//for the unit test
u_char* packets[1000000]; 
int packet_len[1000000];
int pkt_index=0;
struct timeval begin_time;
struct timeval end_time;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
    	printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n", ret);
}

/* Initialize the enclave:
 *   Step 1: try to retrieve the launch token saved by last transaction
 *   Step 2: call sgx_create_enclave to initialize an enclave instance
 *   Step 3: save the launch token if it is updated
 */
int initialize_enclave(void)
{
    char token_path[MAX_PATH] = {'\0'};
    sgx_launch_token_t token = {0};
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int updated = 0;
    
    /* Step 1: try to retrieve the launch token saved by last transaction 
     *         if there is no token, then create a new one.
     */
    /* try to get the token saved in $HOME */
    const char *home_dir = getpwuid(getuid())->pw_dir;
    
    if (home_dir != NULL && 
        (strlen(home_dir)+strlen("/")+sizeof(TOKEN_FILENAME)+1) <= MAX_PATH) {
        /* compose the token path */
        strncpy(token_path, home_dir, strlen(home_dir));
        strncat(token_path, "/", strlen("/"));
        strncat(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME)+1);
    } else {
        /* if token path is too long or $HOME is NULL */
        strncpy(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME));
    }

    FILE *fp = fopen(token_path, "rb");
    if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL) {
        printf("Warning: Failed to create/open the launch token file \"%s\".\n", token_path);
    }

    if (fp != NULL) {
        /* read the token from saved file */
        size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);
        if (read_num != 0 && read_num != sizeof(sgx_launch_token_t)) {
            /* if token is invalid, clear the buffer */
            memset(&token, 0x0, sizeof(sgx_launch_token_t));
            printf("Warning: Invalid launch token read from \"%s\".\n", token_path);
        }
    }
    /* Step 2: call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        if (fp != NULL) fclose(fp);
        return -1;
    }

    /* Step 3: save the launch token if it is updated */
    if (updated == FALSE || fp == NULL) {
        /* if the token is not updated, or file handler is invalid, do not perform saving */
        if (fp != NULL) fclose(fp);
        return 0;
    }

    /* reopen the file with write capablity */
    fp = freopen(token_path, "wb", fp);
    if (fp == NULL) return 0;
    size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);
    if (write_num != sizeof(sgx_launch_token_t))
        printf("Warning: Failed to save launch token to \"%s\".\n", token_path);
    fclose(fp);
    return 0;
}

/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}

void pcap_handle(u_char* user,const struct pcap_pkthdr* header,const u_char* pkt_data)
{
    memcpy(packet,pkt_data,header->caplen);

    //int retval;
    //PacketCount(global_eid, &retval, packet, header->caplen);
    flowCounter(global_eid, packet);
    // forwarding
    //pcap_sendpacket(phandle,packet,header->len);
    counter++;
}

void stop(int signo)
{
    end_t = clock();
    double total;
    total = (double)(end_t - begin_t)/CLOCKS_PER_SEC;
    printf("\n\n**************************************************\n");
    printf("%ld are forwarded ! Average Tput is %f \n",counter, counter / total);
    printf("counter = %ld   begin = %ld  end = %ld \n", counter,begin_t ,end_t);
    printf("**************************************************\n\n");

    /* Destroy the enclave */
    sgx_destroy_enclave(global_eid);
    
    printf("Info: SampleEnclave successfully returned.\n");

    //printf("Enter a character before exit ...\n");
    //getchar();

    _exit(0);
}


int read_local_pcap(int num_flow){

    char filePath[100];
    struct pcap_pkthdr hdr;
    const u_char* tmp;
    pcap_t *descr;

    //long int num_flow = atol(argv[1]);

    sprintf(filePath,"/home/evan/Desktop/%ld_%ld_512.pcap",num_flow,1000000 / num_flow);
    printf("%s\n",filePath);

    char errbuf[PCAP_ERRBUF_SIZE];

    //printf("open the output device success !\n");
    if((descr = pcap_open_offline(filePath,errbuf)) == NULL)
    {
        printf("error !\n");
        return -1;
    }

    packets[pkt_index]  = (u_char *)malloc(MAX_PACKET_LEN * sizeof(u_char));

    tmp = pcap_next(descr,&hdr);
     packet_len[pkt_index] = hdr.caplen;
   
    while(tmp!=NULL){
        memcpy(packets[pkt_index], tmp, hdr.caplen);
        
        // if(pkt_index == 0 || pkt_index ==100000 ||pkt_index == 1000000)
        //     decodePkt(packets[pkt_index]);
        pkt_index++;
        //printf("%d  \n",pkt_index);
        packets[pkt_index]  = (u_char *)malloc(MAX_PACKET_LEN * sizeof(u_char));
        tmp = pcap_next(descr,&hdr);
        packet_len[pkt_index] = hdr.caplen;
    }

    //printf("trace %d :\n    len = %d Bytes\n",pkt_index+1,packetSize[pkt_index]);
    pkt_index = 1000000;
    printf("read finish!read %d packets\n", pkt_index);
    return 0;

}




/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);


    /* Initialize the enclave */
    if(initialize_enclave() < 0){
        printf("Enter a character before exit ...\n");
        getchar();
        return -1; 
    }
 
    /* Utilize edger8r attributes */
    edger8r_array_attributes();
    edger8r_pointer_attributes();
    edger8r_type_attributes();
    edger8r_function_attributes();
    
    // printf("counter start...\n");
    // char errbuf[1024];
    // phandle = pcap_open_live("enp0s31f6",100,1,100,errbuf);//change ens33 to your interface name
    packet  = new u_char[MAX_PACKET_LEN];
    // signal(SIGINT,stop);
    // begin_t   = clock();
    // pcap_loop(phandle,-1,pcap_handle,NULL);

///////unit test////////////
    int pkt_size[6];
    pkt_size[0] = 10;
    pkt_size[1] = 100;
    pkt_size[2] = 1000;
    pkt_size[3] = 5000;
    pkt_size[4] = 10000;
    pkt_size[5] = 20000;
    FILE *fp = fopen("./data.txt","a");
    
    for(int i = 0; i < 6; i++){

        pkt_index = 0;
        read_local_pcap(pkt_size[i]);
        printf("read pcap succeed!\n");
        fprintf(fp, "%dbytes:\n", pkt_size[i]);

        for (int j = 0; j < 5; ++j)
        {
           gettimeofday(&begin_time,NULL);

          for(int k=0;k<pkt_index;k++){
            //printf("a\n");
            //pcap_handle(NULL, pkthdr_set[k], packet_set[k]);
            //PacketCount(packets[k]);
           flowCounter(global_eid, packets[k]);
          }

        gettimeofday(&end_time,NULL);

          double total = end_time.tv_sec-begin_time.tv_sec + (end_time.tv_usec-begin_time.tv_usec)/1000000.0;
          

          fprintf(fp,"\t%f\t%f\tsum = %ld\n",total, (double)pkt_index / total,pkt_index);

          printf("Processed %ld packets: \t%f pps     time = %f\n ", 
            pkt_index, (double)pkt_index / total,total);
        }

        for(int k=0;k<pkt_index;k++){
            //free(pkthdr_set[k]);
            free(packets[k]);
          }
    }
   

    fclose (fp);

    /* Utilize trusted libraries */
    ecall_libc_functions();
    ecall_libcxx_functions();
    ecall_thread_functions();

    /* Destroy the enclave */
    sgx_destroy_enclave(global_eid);
    
    printf("Info: SampleEnclave successfully returned.\n");

    printf("Enter a character before exit ...\n");
    getchar();
    return 0;
}

