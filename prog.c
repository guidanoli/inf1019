  
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
    printf("Processo filho %d pronto para ser executado.\n",pid);
    #endif
    kill(ppid,SIGUSR2); // I am running!
    while( locked );
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
	      #ifdef _DEBUG
        printf("Processo filho %d manda um sinal para pai para entrar em IO...\n",pid);
	      #endif
	      kill(ppid,SIGUSR1); // I am entering IO!
	      sleep(1);           // make sure not to continue
      }
	  }
    #ifdef _DEBUG
    printf("Processo filho %d manda um sinal para pai para terminar...\n",pid);
    #endif
	  kill(ppid,SIGUSR2); // I want to exit!
	  sleep(1);
	  return 0;
  }
  
