#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PER_CALL 1024
#define PAGE_NUM 6000

int *p[PAGE_NUM];
int main(int argc, char *argv[])
{
    printf("Memory Swap test started.\n");

    for(int i=0;i<PAGE_NUM;i++)
    {
      p[i]=(int*)malloc(sizeof(int) * NUM_PER_CALL);
      if(p[i]==0)
        printf("p[i]=0\n");
      for(int j=0;j<NUM_PER_CALL;j++)
      {
        p[i][j]=j;
      }
      if(i+1>=5900)
        printf("%d\n",i+1);
    }
    printf("FREE\n");
    for(int i=0;i<PAGE_NUM;i++)
    {
      if(i%100==0)
        printf("%d\n",i);
      if(p[i]!=0)
        free((void*)p[i]);
    }

    printf("Memory Swap test finished.\n");
    exit(0);
}
