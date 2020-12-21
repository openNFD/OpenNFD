#include "Enclave.h"
#include "Enclave_t.h"  /* print_string */

#define COUNTER_LIST_LEN 2000
#define ETH_HEADER_LEN 14
#define IP_OFFSET 12 

CounterNode* counterList[COUNTER_LIST_LEN];

void flowCounter(unsigned char* packet)
{
    long IP   = *(long *)(packet+ETH_HEADER_LEN+IP_OFFSET);
    //const char* tem;
    //sprintf(tem,"%ld\n",IP);
    //ocall_print_string(tem);
    int index = IP % COUNTER_LIST_LEN; 
    if(index < 0 )
        index = -index; 
    if (counterList[index] == NULL)
    {
        counterList[index]          = (CounterNode *)malloc(sizeof(CounterNode));
        counterList[index]->key     = IP;
        counterList[index]->counter = 1;
        counterList[index]->next    = NULL;
        return ;
    }
    CounterNode *p = counterList[index];
            //ocall_print_string("p->key = %ld\n",p->key);
    do
    {
        if (p->key == IP)
        {
            ////ocall_print_string("p->key = %ld\n",p->key);
            p->counter++;
            //ocall_print_string("counter = %d\n",p->counter);
            break;
        }
        //ocall_print_string("outter\n");
        p = p->next;
    }while(p != NULL);
    
    if (p == NULL)
    {
        //ocall_print_string("NULL\n");
        CounterNode *temp = (CounterNode*)malloc(sizeof(CounterNode));
        temp->key         = IP;
        temp->counter     = 1;
        temp->next        = counterList[index]->next;
        counterList[index]->next = temp;
    }   
    //ocall_print_string("chao\n");
}
    
void initCounterList()
{
    int i = 0;
    for (i = 0;i < COUNTER_LIST_LEN;i++)
    {
        counterList[i] = NULL;
    }
}

void getStat()
{
    int index = 0;
    int flowNum = 0;
    for (index = 0;index < COUNTER_LIST_LEN;index++)
    {
        CounterNode * p = counterList[index];
        while(p != NULL)
        {
            flowNum++;
            long temp = (p->key);
            //ocall_print_string("flow : key = %ld   counter = %d\n",temp,p->counter);
        //  char* pkt_ip = (char* )(&temp);
        //  //ocall_print_string("%2x.%2x.%2x.%2x   ->  %2x.%2x.%2x.%2x    pkt_num = %d\n",pkt_ip[0],pkt_ip[1],pkt_ip[2],pkt_ip[3],pkt_ip[4],pkt_ip[5],pkt_ip[6],pkt_ip[7],p->counter);
            p = p->next;
        }
        
    }           
    //ocall_print_string("FlowNum = %d\n",flowNum);
}
