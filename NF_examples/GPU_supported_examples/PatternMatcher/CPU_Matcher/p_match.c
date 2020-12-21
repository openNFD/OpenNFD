#include <stdio.h>
#include <string.h>
#include <stdlib.h>






extern "C" int p_match( char* haystack, int LENGTH, int num_sub, int len, char* subs)
{
  char *position[num_sub];
  char *h_position[num_sub];
//  genRandomString(haystack,LENGTH);
//  genRandomSubString(subs,len*num_sub,len);

  memset(h_position,0,sizeof(char*)*num_sub);
  const size_t smem = sizeof(char)*LENGTH;
  char h_subs[num_sub][len];
  for(int i=0;i<num_sub;++i){
    for(int j=0;j<len;++j){
      h_subs[i][j] = subs[i*len+j];
    }
  }
    

  for(int i=0;i<num_sub;++i)
  { 
    char* ret;
    //printf("string is %s and sub is %s\n",haystack,h_subs[i]);
    ret = strstr(haystack,h_subs[i]);
    if(ret != NULL){
      //printf("find one sub string in %d sub\n",i);
      //printf("%s\n",ret);
    }
    position[i] = ret;
  }
  return(0);
}
