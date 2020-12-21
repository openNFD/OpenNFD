#include <stdlib.h>
#include <string.h>

class Flow {
    unsigned char * pkt;
public:
    Flow(unsigned char * pkt);
    void clean();

    int sip, dip;
};


const int TABLE_LEN = 9973;

template <typename T>
class State {
private:
    //T init;
    T table[TABLE_LEN];
public: 
	/*Initial*/
	/* count is number of fields/keywords to distinct two state instances*/
	State(T ini, int flag) {
		//this->init=ini;

        if(flag){
            //T is a kind of pointer 
            //initiate the table
            int size = strlen(ini);
            for (int i = 0; i < TABLE_LEN; ++i){
                table[i] = (T) malloc(20);
                memcpy(table[i], ini, size);
                table[i][size] = '\0';
            }
         }   
         else{
            //not a pointer
            for (int i = 0; i < TABLE_LEN; ++i)
                table[i] = ini;
         }

		return;
	}

    int get_key(Flow& f) {
        int key = ((((uint64_t)f.sip)<<32)|f.dip)%TABLE_LEN;
        return key;
    }
	
	/* [] return states of type T belonging to f*/
	T& operator[](Flow& f) {
        int key = ((((uint64_t)f.sip)<<32)|f.dip)%TABLE_LEN;
        return table[key];
	}

};
