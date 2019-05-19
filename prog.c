  
  // Processo usuário
  // Guilherme Dantas

  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <signal.h>

  int locked = 1;

  void handler(int signo) {
    locked = 0;
  }

  int main(int argc, char ** argv)
  {
    int ppid = getppid(), pid = getpid();
    signal(SIGCONT,handler);
    #ifdef _DEBUG
    printf("Processo filho %d chama SIGUSR2 ao pai...\n",pid);
    #endif
    kill(ppid,SIGUSR2); // I am running!
    while( locked );
    #ifdef _DEBUG
    printf("Processo filho %d rodando!\n",pid);
    #endif
    signal(SIGCONT,SIG_DFL);
	  for( int i = 1 ; i < argc ; i++ )
	  {
	    for( int j = 0 ; j < atoi(argv[i]) ; j++ )
	    {
	      printf("%d\n",pid);
	      sleep(1); // CPU
	    }
	    if( i < argc - 1 )
	    {
        printf("%d entrou em IO!\n",pid);
	      kill(ppid,SIGUSR1); // I am entering IO!
      }
	  }
	  return 0;
  }
  
