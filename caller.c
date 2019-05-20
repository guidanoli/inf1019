  
  #include <stdio.h>
  #include <unistd.h>
  #include <signal.h>
  #include <sys/types.h>
  
  int loop = 0;
  int status = 0;
  
  void handler(int signo)
  {
    if( signo == SIGUSR1 )
    {
      printf("IO!\n");
      status = 1;
    }
    else if( signo == SIGUSR2 )
    {
      printf("TERMINATED!\n");
      status = 2;
    }
  }
  
  int main(void)
  {
    pid_t pid = fork();
    
    if( pid == 0 )
      execl("doer","doer",NULL);
    else
    {
      sleep(1);
      kill(pid,SIGSTOP);
      signal(SIGUSR1,handler);
      signal(SIGUSR2,handler);
      printf("Entering loop\n");
      while(1)
      {
        printf("Loop #%d\n",loop);
        sleep(1);
        kill(pid,SIGCONT);
        sleep(3);
        kill(pid,SIGSTOP);
        sleep(1);
        if( status == 1 )
        {
          sleep(3);
        }
        if( status == 2 )
        {
          printf("Exit...\n");
          exit(0);
        }
        status = 0;
        loop++;
      }
    }
    
    return 0;
  }
  
