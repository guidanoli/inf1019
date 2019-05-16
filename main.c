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
#define PROG_FILENAME "prog"

qhead filas[QTD_FILAS];
qnode current_process = NULL;

int main(int argc, char ** argv)
{
  char c;
  int status = 0;
  pid_t pid;
  
  for( int i = 0 ; i < QTD_FILAS ; i++ )
  {
    if( qhead_create(filas,i+1) != 0 )
    {
      fprintf(stderr,"Não foi possível alocar espaço na memória.\n");
      return EXIT_FAILURE;
    }
  }
  
  while((c = fgetc(stdin)) == 'e')
  {
    char prog[STR_PROGNAME_MAX] = "";
    int r[3];
    
    if( scanf("xec %s (%d,%d,%d)\n",prog,r,r+1,r+2) != 4 )
    {
      fprintf(stderr,"String mal formatada.\n");
      return EXIT_FAILURE;
    }
    
    scanf(" ");
    
    #ifdef _DEBUG
    printf("Carregado programa %s com rajadas %d, %d e %d.\n",prog,r[0],r[1],r[2]);
    #endif
    
    if((pid = fork()) == 0)
    {
      sleep(1);
      char r1[16];
      char r2[16];
      char r3[16];
      sprintf(r1,"%d",r[0]);
      sprintf(r2,"%d",r[1]);
      sprintf(r3,"%d",r[2]);
      execl(PROG_FILENAME,PROG_FILENAME,r1,r2,r3,(char *)NULL);
    }
    else
    {
      kill(pid,SIGSTOP);
      qnode process;
      printf("Capturei o processo de pid %d\n",pid);
      if( qnode_create(&process,pid) != 0 )
      {
        fprintf(stderr,"Não foi possível alocar espaço na memória.\n");
        return EXIT_FAILURE;
      }
      qhead_ins(filas[0],process);
    }
    
  }
  
  while((current_process = qhead_rm(filas[0]))!=NULL)
  {
    pid = qnode_getid(current_process);
    printf("Recuperado processo de pid %d\n",pid);
    kill(pid,SIGCONT);
    waitpid(pid,&status,0);
    qnode_destroy(&current_process);
  }
  qhead_destroy(filas);
  qhead_destroy(filas+1);
  qhead_destroy(filas+2);
  
	return EXIT_SUCCESS;
}
