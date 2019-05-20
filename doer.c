  
  #include <stdio.h>
  #include <unistd.h>
  #include <signal.h>
  #include <sys/types.h>
    
  int main(void)
  {
    for( int i = 0 ; i < 3 ; i++ )
    {
      for( int j = 0 ; j < 2 ; j++ )
      {
        printf("%d-%d\n",i,j);
        sleep(1);
      }
      if( i < 2 )
      {
        printf("I am calling IO!\n");
        kill(getppid(),SIGUSR1);
        sleep(1);
      }
    }
    printf("I want to exit!\n");
    kill(getppid(),SIGUSR2);
    return 0;
  }
  
