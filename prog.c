  
  // Processo usuário
  // Guilherme Dantas

  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <signal.h>

  int main(int argc, char ** argv)
  {
    int ppid = getppid(), pid = getpid();
	  for( int i = 1 ; i < argc ; i++ )
	  {
	    for( int j = 0 ; j < atoi(argv[i]) ; j++ )
	    {
	      printf("%d\n",pid);
	      sleep(1); // CPU
	    }
	    if( i < argc - 1 )
	    {
        printf("%d [io]\n",pid);
	      kill(ppid,SIGUSR1); // I am entering IO!
      }
	  }
	  return 0;
  }
  
