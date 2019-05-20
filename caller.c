  
  #include <stdio.h>
  #include <unistd.h>
  #include <signal.h>
  #include <sys/types.h>
  #include <pthread.h>
  #include "queue.h"
  #include "semlib.h"
  
  #define KEY 12345
  
  int loop = 0;
  int status = 0;
  int locked = 1;
  qhead signal_queue;
  int semId;
  pthread_t thread;
  
  void handler(int signo)
  {
    qnode sig;
    enterCR(semId);
    printf("Entering signo %d in queue...\n",signo);
    qnode_create(&sig,signo);
    qhead_ins(signal_queue,sig);
    exitCR(semId);
  }
  
  void * ThreadFunc( void * info )
  {
    sleep(100);
    printf("Exiting thread...\n");
    pthread_exit(NULL);
  }
  
  int main(void)
  {
    pid_t pid = fork();
    
    if( pid == 0 )
      execl("doer","doer",NULL);
    else
    {
      kill(pid,SIGSTOP);
      signal(SIGUSR1,handler);
      signal(SIGUSR2,handler);
      semId = semCreate(KEY);
      if( semInit(semId) == -1 )
      {
        fprintf(stderr,"Error on sem\n");
        exit(1);
      }
      qhead_create(&signal_queue,1);
      pthread_create(&thread,NULL,ThreadFunc,NULL);
      printf("Entering loop\n");
      while(status==0)
      {
        printf("Loop #%d\n",loop);
        kill(pid,SIGCONT);
        sleep(3);
        kill(pid,SIGSTOP);
        while(qhead_empty(signal_queue)==QUEUE_FALSE)
        {
          qnode sig;
          int signo;
          enterCR(semId);
          sig = qhead_rm(signal_queue);
          signo = qnode_getid(sig);
          qnode_destroy(&sig);
          exitCR(semId);
          if( signo == SIGUSR1 )
          {
            printf("IO!\n");
            sleep(3);
          }
          if( signo == SIGUSR2 )
          {
            printf("Exit...\n");
            kill(pid,SIGKILL);
            status = 1;
          }
        }
        loop++;
      }
    }
    semDestroy(semId);
    return 0;
  }
  
