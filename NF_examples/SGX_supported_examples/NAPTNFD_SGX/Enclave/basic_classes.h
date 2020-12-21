#include <string.h>
#include <stdlib.h>
#include <stdio.h>


class Flow {
    unsigned char * pkt;
public:
    Flow(unsigned char * pkt);
    void clean();

    int sip, dip;
    unsigned short dport;

    unsigned short sport;
};

const int TABLE_LEN = 65535;

template <typename T>
class State {
private:
    //T init;
    T table[TABLE_LEN];
public:
	/*Initial*/
	/* count is number of fields/keywords to distinct two state instances*/
	State(char * ip_address, int port) {
		//this->init=ini;
        int size = strlen(ip_address);
        for (int i = 0; i < TABLE_LEN; ++i){
            table[i].port = 0;
            memcpy(table[i].inetIP, ip_address, size);
            table[i].inetIP[size] = '\0';
        }

		return;
	}

    int get_key(Flow& f) {
        // int key = ((((uint64_t)f.sip)<<32)|f.dip)%TABLE_LEN;
        int key  = f.dport;
        return key;
    }
	
	/* [] return states of type T belonging to f*/
	T& operator[](Flow& f) {
        // int key = ((((uint64_t)f.sip)<<32)|f.dip)%TABLE_LEN;
        int key = f.dport;
        return table[key];
	}

};

typedef struct IP_Port_pair{
    char inetIP[20];
    int port;
}IP_Port_pair;