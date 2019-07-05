
  // User process
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
        sleep(1);
	      printf("%d\n",pid);
        kill(ppid,SIGUSR1); // One second has passed!
	    }
	    if( i < argc - 1 )
	    {
	      sleep(1);           // make sure not to continue
      }
	  }
	  return 0;
  }
