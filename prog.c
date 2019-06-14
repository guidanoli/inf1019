
  // Processo usuário
  // Guilherme Dantas

  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <signal.h>

  int main(int argc, char ** argv)
  {
    int ppid = getppid(), pid = getpid();
    #ifdef _DEBUG
    printf("Child process %d ready to be executed.\n",pid);
    #endif
	  for( int i = 1 ; i < argc ; i++ )
	  {
	    for( int j = 0 ; j < atoi(argv[i]) ; j++ )
	    {
	      printf("%d\n",pid);
	      sleep(1); // CPU
        kill(ppid,SIGUSR1); // I am entering IO!
	    }
	    if( i < argc - 1 )
	    {
	      #ifdef _DEBUG
        printf("Child process %d requests parent to block him (IO)...\n",pid);
	      #endif
	      sleep(1);           // make sure not to continue
      }
	  }
    #ifdef _DEBUG
    printf("Child process %d requests parent to terminate him (EOF)...\n",pid);
    #endif
	  return 0;
  }
