// Interpretador e Escalonador
// Guilherme Dantas

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <string.h>
#include "queue.h"

#define QTD_FILAS 3
#define ARG_SIZE 64
#define QTD_ARGS 16

qhead filas[QTD_FILAS];
qnode current_process = NULL;

static int fatal_error(const char * err_msg_format, ...)
{
  va_list vl;
  va_start(vl, err_msg_format);
  vfprintf(stderr,err_msg_format,vl);
  va_end(vl);
  return EXIT_FAILURE;
}

static int create_queues(void)
{
  for( int i = 0 ; i < QTD_FILAS ; i++ )
  {
    if( qhead_create(filas+i,i+1) != 0 )
      return fatal_error("Não foi possível alocar espaço na memória.\n");
  }
  return 0;
}

static void destroy_queues(void)
{
  for( int i = 0 ; i < QTD_FILAS ; i++ )
  {
    qhead_destroy(filas+i);
  }
}

int main(int argc, char ** argv)
{
  int status = 0, ret;
  char c;
  pid_t pid;
  
  if((ret=create_queues())!=0) return ret;
  
   while((c = fgetc(stdin)) == 'e')
   {
     char prog[ARG_SIZE] = "";
     int raj[QTD_ARGS];
     int qt_raj = 1;
     
     if( scanf("xec %s (%d",prog,raj) != 2 )
     { 
       return fatal_error("String mal formatada.\n");
   }
   while((c = fgetc(stdin)) == ',')
   {
     if( scanf("%d",raj+qt_raj) != 1 )
     {
       return fatal_error("String mal formatada.\n");
     }
     qt_raj++;
   }
   if( c != ')' )
   {
     return fatal_error("String mal formatada.\n");
   }
   scanf(" ");
   
   if((pid = fork()) == 0)
   {
     char * args[QTD_ARGS];
     for( int i = 0 ; i < qt_raj+2 ; i++ )
     {
       args[i] = (char *) malloc(sizeof(char)*ARG_SIZE);
       if( i == 0 ) strcpy(args[0],prog);
       else if( i < qt_raj+1 ) sprintf(args[i],"%d",raj[i-1]);
       #ifdef _DEBUG
       if( i < qt_raj+1 ) printf("args[%d] = %s\n",i,args[i]);
       #endif
     }
     args[qt_raj+1] = NULL;
     #ifdef _DEBUG
     printf("Entrando...\n");
     #endif
     if( execv(*args,args) == -1 )
     {
       return fatal_error("Could not execute program %d.\n",getpid());
     }
   }
   else
   {
     #ifdef _DEBUG
     printf("Fazendo o processo filho parar\n");
     #endif
     kill(pid,SIGSTOP);
     qnode process;
     if( qnode_create(&process,pid) != 0 )
     {
       fprintf(stderr,"Não foi possível alocar espaço na memória.\n");
       return EXIT_FAILURE;
     }
     qhead_ins(filas[0],process);
     #ifdef _DEBUG
     printf("[%d] inserido em F1\n",pid);
     #endif
   }
   
  }
  
  while((current_process = qhead_rm(filas[0]))!=NULL)
  {
    pid = qnode_getid(current_process);
    #ifdef _DEBUG
    printf("[%d] retirado de F1\n",pid);
    #endif
    kill(pid,SIGCONT);
    waitpid(pid,&status,0);
    qnode_destroy(&current_process);
  }
  
  destroy_queues();
  
	return EXIT_SUCCESS;
}
