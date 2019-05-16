// Processo usuário
// Guilherme Dantas

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char ** argv)
{
	for( int i = 1 ; i < argc ; i++ )
	{
	  for( int j = 0 ; j < atoi(argv[i]) ; j++ )
	  {
	    printf("%d\n",getpid());
	    sleep(1); // CPU
	  }
	  if( i < argc - 1 ) sleep(3); // IO
	}
	return 0;
}
