#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cuda.h>
#include <device_functions.h>
#include <time.h> 
__global__ void my_strstr(char *str,char *sub_string,char ** position,int str_len,int sub_len,int num_sub)
{
  int id = threadIdx.x;
  //char *sub = &sub_string[id*sub_len];
  char *result = NULL; 
  char sub[24];
  //load sub in register,great improve
  for(int i=0;i<sub_len;++i){
    sub[i] = sub_string[id*sub_len+i];}
  //best case using Shared memory
  extern __shared__ char s_string[];
  //every thread has to fetch how many values from global memory to shared memory
  int each_num = str_len/blockDim.x;
  for(int i=0;i<each_num;++i){
    s_string[i*blockDim.x+id] = str[i*blockDim.x+id];}
  if( ((each_num*blockDim.x+id) < str_len) && (blockDim.x > each_num) )
    s_string[each_num*blockDim.x+id] = str[each_num*blockDim.x+id];
  __syncthreads();
  char *string = s_string;
//  char *length = s_string;
//  int i;
//  for(i = 0; length[i] != '\0'; i++);
//  printf("length is %d\n",i);
  char *a,*b;
  //b point to the sub address in register rather than in global memory
  b = sub;
  //result == NULL to judge if we find a match;rather than use goto or break in loop which harm the calculation
  int i;
  for(i = 0;(i < str_len)&&(result == NULL);i++){
    //printf("i am %d\n",id);
    a = string;
    while(*a++ == *b++){
      if(*(b+1) == '\0'){ 
        result = string;
      }
    }
    b = sub;
    ++string;
  }
  //coalesced global memory store, no effect since we only store once 
  position[id] = result;
//  printf("12213\n");
}



extern "C" int p_match( char* haystack, int LENGTH, long num_sub, int len, char* subs)
{
//  printf("Length is %d\n",LENGTH);
  int num_block,num_thread;
  if(num_sub < 512){
    num_block = 1;
    num_thread = num_sub;
  }
  else{
    num_block = num_sub / 512;
    num_thread = 512;
  }
  char *h_position[num_sub];

  char *d_string,*d_subs;
  char **d_position;
  cudaMalloc((void**)&d_string,sizeof(char)*LENGTH);
  cudaMalloc((void**)&d_subs,sizeof(char)*num_sub*len);
  cudaMalloc((void***)&d_position,sizeof(char*)*num_sub);
  cudaMemset(d_position,0,sizeof(char*)*num_sub);
  memset(h_position,0,sizeof(char*)*num_sub);
  const size_t smem = sizeof(char)*LENGTH;

  /*GPU*/
    cudaMemcpy(d_string,haystack,sizeof(char)*LENGTH,cudaMemcpyHostToDevice);
    cudaMemcpy(d_subs,subs,sizeof(char)*num_sub*len,cudaMemcpyHostToDevice);
//    printf("num_block is %d  and num_thread is %d\n",num_block,num_thread);
    my_strstr<<<num_block,num_thread,smem>>>(d_string,d_subs,d_position,LENGTH,len,num_sub);
    cudaDeviceSynchronize();
    cudaMemcpy(h_position,d_position,sizeof(char*)*num_sub,cudaMemcpyDeviceToHost);

  return(0);
}
