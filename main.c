// Interpretador e Escalonador
// Guilherme Dantas

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "queue.h"

#define QTD_FILAS 3
#define STR_PROGNAME_MAX 64

qhead filas[QTD_FILAS];
qnode current_process = NULL;

int main(int argc, char ** argv)
{
  char c;
  int status = 0;
  pid_t pid;
  
  /* creating queues */
  for( int i = 0 ; i < QTD_FILAS ; i++ )
  {
    if( qhead_create(filas+i,i+1) != 0 )
    {
      fprintf(stderr,"Não foi possível alocar espaço na memória.\n");
      return EXIT_FAILURE;
    }
  }
  
  /* stdin processing */
  while((c = fgetc(stdin)) == 'e')
  {
    char prog[STR_PROGNAME_MAX] = "";
    int r[3];
    
    if( scanf("xec %s (%d,%d,%d)\n",prog,r,r+1,r+2) != 4 )
    {
      fprintf(stderr,"String mal formatada.\n");
      return EXIT_FAILURE;
    }
    
    scanf(" "); // wait until next non-whitespace character
    
    #ifdef _DEBUG
    printf("Carregado programa %s com rajadas %d, %d e %d.\n",prog,r[0],r[1],r[2]);
    #endif
    
    if((pid = fork()) == 0)
    {
      char r0[16] = "";
      char r1[16] = "";
      char r2[16] = "";
      sprintf(r0,"%d",r[0]);
      sprintf(r1,"%d",r[1]);
      sprintf(r2,"%d",r[2]);
      execl(prog,prog,r0,r1,r2,(char *)NULL);
    }
    else
    {
      kill(pid,SIGSTOP);
      qnode process;
      if( qnode_create(&process,pid) != 0 )
      {
        fprintf(stderr,"Não foi possível alocar espaço na memória.\n");
        return EXIT_FAILURE;
      }
      qhead_ins(filas[0],process);
      printf("[%d] inserido em F1\n",pid);
    }
    
  }
  
  /* waking up processes from F1 */
  while((current_process = qhead_rm(filas[0]))!=NULL)
  {
    pid = qnode_getid(current_process);
    printf("[%d] retirado de F1\n",pid);
    kill(pid,SIGCONT);
    waitpid(pid,&status,0);
    qnode_destroy(&current_process);
  }
  qhead_destroy(filas);
  qhead_destroy(filas+1);
  qhead_destroy(filas+2);
  
	return EXIT_SUCCESS;
}
