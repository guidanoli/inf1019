
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
  #define SEM_KEY 123456789

  // variables
  qhead filas[QTD_FILAS];
  qhead aux_queue = NULL;
  qnode current_process = NULL;
  int current_queue_id = 0;
  int queue_runs_left = 0;
  int io_processes = 0;
  int semId = 0;
  
  // functions
  int fatal_error(const char * err_msg_format, ...);
  int create_queues(void);
  int init_interpreter();
  void destroy_queues(void);
  int myPow2(int exp);
  int getQueueRuns(int id);
  int getQueueQuantum(int id);
  qhead getCurrentQueue();
  void setCurrentQueue(int id);

  int main(int argc, char ** argv)
  {
    int ret;
    
    /* create queues */
    if((ret=create_queues())!=0) return ret;
    
    /* parse args from stdin and build queues */
    if((ret=init_interpreter())!=0) return ret;
    
    /* initialize semaphore */
    semId = semCreate(SEM_KEY);
    
    /* start from queue of highest priority */
    setCurrentQueue(0);
    
    /* main loop */
    while( 1 )
    {
      break;
    }
    
    /* safely destroying semaphore */
    semDestroy(semId);
    
    /* safely freeing queues */
    destroy_queues();
    
    /* end scheduler */
	  return EXIT_SUCCESS;
  }
  
  int fatal_error(const char * err_msg_format, ...)
  {
    va_list vl;
    va_start(vl, err_msg_format);
    vfprintf(stderr,err_msg_format,vl);
    va_end(vl);
    return EXIT_FAILURE;
  }
  
  int create_queues(void)
  {  
    if( qhead_create(&aux_queue,0) != 0 )
        return fatal_error("Falha ao criar fila auxiliar.\n");
    for( int i = 0 ; i < QTD_FILAS ; i++ )
      if( qhead_create(filas+i,i+1) != 0 )
        return fatal_error("Falha ao crier fila #%d.\n",i+1);
    return 0;
  }

  void destroy_queues(void)
  {
    qhead_destroy(aux_queue);
    for( int i = 0 ; i < QTD_FILAS ; i++ )
      qhead_destroy(filas+i);
  }
  
  qhead getCurrentQueue()
  {
    if( queue_runs_left == 0 )
      setCurrentQueue((current_queue_id+1)%QTD_FILAS);
    queue_runs_left--;
    return filas[current_queue_id];
  }
  
  void setCurrentQueue(int id)
  {
    current_queue_id = id;
    queue_runs_left = getQueueRuns(id);
  }
  
  int myPow2(int exp) { return 1<<exp; }
  
  int getQueueRuns(int id) { return myPow2(QTD_FILAS-id); }
  
  int getQueueQuantum(int id) { return myPow2(QTD_FILAS); }
      
  int init_interpreter()
  {
    char args[QTD_ARGS][ARG_SIZE], prog[ARG_SIZE], c;
    int raj[QTD_ARGS];
    pid_t pid;
    while((c = fgetc(stdin)) == 'e')
    {
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
        for( int i = 0 ; i < qt_raj+2 ; i++ )
        {
          if( i == 0 ) strcpy(args[0],prog);
          else if( i < qt_raj+1 ) sprintf(args[i],"%d",raj[i-1]);
          #ifdef _DEBUG
          if( i < qt_raj+1 ) printf("args[%d] = %s\n",i,args[i]);
          #endif
        }
        strcpy(args[qt_raj+1],'\0');
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
        /* inserts all processes in the high-priority queue */
        qhead_ins(filas[0],process);
        #ifdef _DEBUG
        printf("[%d] inserido em F1\n",pid);
        #endif
      }
    } /* end parsing */
    return 0;
  }
  
  
  
  
  
  
