
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
  #include <pthread.h>
  #include "queue.h"
  
  #define SMALLEST_QUANTUM 1
  #define IO_BLOCK_TIME 3
  #define N_OF_QUEUES 3
  #define ARG_SIZE 64
  #define QTD_ARGS 16
  #define SEM_KEY 123456789
  
  typedef enum {
    NORMAL,
    IO,
    TERMINATED,
  } proc_status;
  
  typedef enum {
    PARSING,
    MANAGING,
    EXECUTING,
  } schdlr_status;
    
  typedef struct process_s {
    qnode node;
    proc_status status;
  } process;
  
  typedef struct process_queue_s {
    qhead queue;
    int id;
    int runs_left;
  } procqueue;
  
  typedef struct process_package_s {
    qnode process;
    qhead queue;
  } procpack;
  
  // processes and queues
  qhead proc_queues[N_OF_QUEUES];
  qhead aux_queue = NULL;
  process current_proc;
  procqueue current_queue;
  schdlr_status my_status = PARSING;
  int processes_count = 0;
  int io_threads = 0;
  int signal_lock = 0;
  
  // semaphore
  int semId = 0;
  
  // functions
  int fatal_error(const char * err_msg_format, ...);
  int create_queues(void);
  int init_interpreter();
  void destroy_queues(void);
  int myPow2(int exp);
  int getQueueRuns(int id);
  int getQueueQuantum(int id);
  int getHigherPriorityQueueId(int id);
  int getLowerPriorityQueueId(int id);
  qhead getQueueFromId(int id);
  qhead getUpdatedQueue();
  void setCurrentQueue(int id);
  void forceNextQueue();
  void signalHandler(int signo);
  void * ioProcessHandler(void * info);
  procpack * getCurrentProcessPackage();

  int main(int argc, char ** argv)
  {
    int ret = 0, quantum = 0;
    qhead queue;
    pid_t pid;
    
    /* create queues */
    if((ret=create_queues())!=0) return ret;
    
    /* initialize semaphore */
    if((semId = semCreate(SEM_KEY))==-1) return EXIT_FAILURE;
    if(semInit(semId)==-1) return EXIT_FAILURE;
    
    /* initialize signal handlers for interpreter */
    signal(SIGUSR2,signalHandler);
    
    /* parse args from stdin and build queues */
    if((ret=init_interpreter())!=0) return ret;
    
    /* change state */
    my_status = MANAGING;
    
    /* initialize signal handlers for scheduler */
    signal(SIGUSR1,signalHandler);
    signal(SIGCHLD,signalHandler);
    signal(SIGUSR2,SIG_DFL);
    
    /* start from queue of highest priority */
    setCurrentQueue(0);
    
    #ifdef _DEBUG
    printf("Beggining main loop...\n");
    #endif
        
    /* main loop */
    while( 1 )
    {
      if( processes_count <= 0 ) break;
      
      // there is no need for mutex here
      // since the processes_count only
      // decreases and it only reads the
      // content of process_count and does
      // not modify it.
      
      queue = getUpdatedQueue();
      quantum = getQueueQuantum(current_queue.id);
      while( qhead_empty(queue) == QUEUE_FALSE )
      {
        /////////////////////////////////
        // ENTERS CRITICAL REGION
        // Manipulates current process
        // and current queue
        /////////////////////////////////
        enterCR(semId);
        /////////////////////////////////
        current_proc.node = qhead_rm(queue);
        current_proc.status = NORMAL;
        pid = qnode_getid(current_proc.node);
        /////////////////////////////////
        exitCR(semId);
        /////////////////////////////////
        // EXITS CRITICAL REGION
        /////////////////////////////////        
        
        signal_lock = 1;
        kill(pid,SIGCONT);
        while(signal_lock);
        my_status = EXECUTING;
        signal_lock = 2; // allows termination
        sleep(quantum); // z z z ...
        
        /////////////////////////////////
        // ENTERS CRITICAL REGION
        // Manipulates current process
        // and current queue
        /////////////////////////////////
        enterCR(semId);
        /////////////////////////////////
        my_status = MANAGING;
        kill(pid,SIGSTOP);
        /////////////////////////////////
        exitCR(semId);
        /////////////////////////////////
        // EXITS CRITICAL REGION
        /////////////////////////////////
        
        while(signal_lock);
        
        /////////////////////////////////
        // ENTERS CRITICAL REGION
        // Manipulates current process
        /////////////////////////////////
        enterCR(semId);
        /////////////////////////////////
        if( current_proc.status == NORMAL )
        {
          int new_queue_id = getLowerPriorityQueueId(current_queue.id);
          printf("Process %d exceeded its quantum.\n",qnode_getid(current_proc.node));
          if( new_queue_id == current_queue.id )
          {
            qhead_ins(aux_queue,current_proc.node);
            printf("Process %d will stay on queue #%d.\n",qnode_getid(current_proc.node),new_queue_id);
          }
          else
          {
            qhead_ins(getQueueFromId(new_queue_id),current_proc.node);
            printf("Process %d will go from queue #%d to #%d\n",qnode_getid(current_proc.node),current_queue.id, new_queue_id);
          }
          current_proc.node = NULL;
        }
        /////////////////////////////////
        exitCR(semId);
        /////////////////////////////////
        // EXITS CRITICAL REGION
        /////////////////////////////////
      }
      if( qhead_transfer(aux_queue,queue,QFLAG_TRANSFER_ALL) != QUEUE_OK )
        return fatal_error("Erro ao administrar filas auxiliares\n");
    }
    
    #ifdef _DEBUG
    printf("Ended.\n");
    #endif
    
    /* safely destroying semaphore */
    semDestroy(semId);
    
    /* safely freeing queues */
    destroy_queues();
    
    /* end scheduler */
	  return EXIT_SUCCESS;
  }
  
  ///////////////////////////////
  // Functions' implementation //
  ///////////////////////////////
  
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
    if( qhead_create(&aux_queue,-1) != 0 )
        return fatal_error("Falha ao criar fila auxiliar.\n");
    for( int i = 0 ; i < N_OF_QUEUES ; i++ )
      if( qhead_create(proc_queues+i,i) != 0 )
        return fatal_error("Falha ao crier fila #%d.\n",i);
    return 0;
  }

  void destroy_queues(void)
  {
    qhead_destroy(&aux_queue);
    for( int i = 0 ; i < N_OF_QUEUES ; i++ )
      qhead_destroy(proc_queues+i);
  }
  
  qhead getQueueFromId(int id)
  {
    return proc_queues[id%N_OF_QUEUES];
  }
  
  qhead getUpdatedQueue()
  {
    if( current_queue.runs_left == 0 ) forceNextQueue();
    current_queue.runs_left--; // already wastes by calling
    return getQueueFromId(current_queue.id);
  }
  
  void forceNextQueue()
  {
    setCurrentQueue((current_queue.id+1)%N_OF_QUEUES);
  }
  
  void setCurrentQueue(int id)
  {
    /////////////////////////////////
    // ENTERS CRITICAL REGION
    // Manipulates current queue
    /////////////////////////////////
    enterCR(semId);
    /////////////////////////////////
    current_queue.id = id;
    current_queue.runs_left = getQueueRuns(id);
    /////////////////////////////////
    exitCR(semId);
    /////////////////////////////////
    // EXITS CRITICAL REGION
    /////////////////////////////////
  }
  
  int myPow2(int exp) { return 1<<exp; }
  
  int getQueueRuns(int id) { return myPow2(N_OF_QUEUES-id-1); }
  
  int getQueueQuantum(int id) { return myPow2(id)*SMALLEST_QUANTUM; }
    
  // [!] NOT SAFE WITH SEMAPHORES - MUST USE WHEN CALLING
  procpack * getCurrentProcessPackage()
  {
    procpack * pack = (procpack *) malloc(sizeof(procpack));
    if( pack == NULL ){
      fprintf(stderr,"Não foi possível alocar espaço na memória.\n");
      exit(0); //abort program
    }
    pack->process = current_proc.node;
    pack->queue = getQueueFromId(getHigherPriorityQueueId(current_queue.id));
    return pack;
  }
  
  void signalHandler(int signo)
  {    
    if( signo == SIGUSR1 )
    {
      int pid;
      void * info;
      pthread_t thread;
      /////////////////////////////////
      // ENTERS CRITICAL REGION
      // Manipulates current process
      // and current queue. Later on,
      // another queue too.
      /////////////////////////////////
      enterCR(semId);
      /////////////////////////////////
      // proccess has entered I/O
      // a thread will handle it properly
      pid = qnode_getid(current_proc.node);
      kill(pid,SIGSTOP);
      info = (void *) getCurrentProcessPackage(); // inside semaphore
      pthread_create(&thread,NULL,ioProcessHandler,info);
      current_proc.status = IO;
      current_proc.node = NULL;
      io_threads++;
      /////////////////////////////////
      exitCR(semId);
      /////////////////////////////////
      // EXITS CRITICAL REGION
      /////////////////////////////////
      #ifdef _DEBUG
      if( io_threads > processes_count )
      {
        printf("#Threads > #Processes ! ! !\n");
        exit(0);
      }
      signal_lock = 0;
      #endif
    }
    else if( signo == SIGUSR2 )
    {
      if( my_status != PARSING )
      {
        qnode dead_node;
        #ifdef _DEBUG
        printf("Goodbye cruel world...\n");
        #endif
        /////////////////////////////////
        // ENTERS CRITICAL REGION
        // Manipulates  the process status
        // flag and the current queue
        /////////////////////////////////
        enterCR(semId);
        /////////////////////////////////
        dead_node = current_proc.node;
        processes_count--;
        qnode_destroy(&dead_node);
        current_proc.status = TERMINATED;
        current_proc.node = NULL;
        /////////////////////////////////
        exitCR(semId);
        /////////////////////////////////
        // EXITS CRITICAL REGION
        /////////////////////////////////
        signal_lock = 0;
      }
    }
  }
  
  void * ioProcessHandler(void * info)
  {
    procpack * pack;
    qnode io_proc;
    qhead new_queue;
    int pid;
    signal(SIGUSR1,SIG_DFL);
    signal(SIGCHLD,SIG_DFL);
    pid = qnode_getid(io_proc);
    pack = (procpack *) info;
    io_proc = pack->process;
    new_queue = pack->queue;
    free(info);
    sleep(IO_BLOCK_TIME); // simulating IO
    /////////////////////////////////
    // ENTERS CRITICAL REGION
    // Manipulates io_process and
    // any of the process queues
    /////////////////////////////////
    enterCR(semId);
    /////////////////////////////////
    qhead_ins(new_queue,io_proc);
    /////////////////////////////////
    exitCR(semId);
    /////////////////////////////////
    // EXITS CRITICAL REGION
    /////////////////////////////////
    pthread_exit(NULL); // end thread
  }
  
  int getHigherPriorityQueueId(int id)
  {
    if(id <= 0) return 0;
    return (id-1)%N_OF_QUEUES;
  }
  
  int getLowerPriorityQueueId(int id)
  {
    if(id >= N_OF_QUEUES-1) return N_OF_QUEUES-1;
    return (id+1)%N_OF_QUEUES;
  }
   
  int init_interpreter()
  {
    char c;
    pid_t pid;
    /* parse args from stdin */
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
        #ifdef _DEBUG
        char buffer[QTD_ARGS*(ARG_SIZE+1)+1] = "";
        #endif
        for( int i = 0 ; i < qt_raj+2 ; i++ )
        {
          args[i] = (char *) malloc(sizeof(char)*ARG_SIZE);
          if( i == 0 ) strcpy(args[0],prog);
          else if( i < qt_raj+1 ) sprintf(args[i],"%d",raj[i-1]);
          #ifdef _DEBUG
          if( i < qt_raj+1 )
          {
            strcat(buffer,args[i]);
            strcat(buffer," ");
          }
          #endif
        }
        args[qt_raj+1] = NULL;
        #ifdef _DEBUG
        printf("Executando... %s\n",buffer);
        #endif
        if( execv(*args,args) == -1 )
        {
          return fatal_error("Could not execute program %d.\n",getpid());
        }
        exit(0);
      }
      else
      {
        sleep(100);
        kill(pid,SIGSTOP);
        qnode process;
        if( qnode_create(&process,pid) != 0 )
        {
          fprintf(stderr,"Não foi possível alocar espaço na memória.\n");
          return EXIT_FAILURE;
        }
        qhead_ins(proc_queues[0],process);
        sleep(1);
        processes_count++;
        printf("process_count = %d\n",processes_count);
        #ifdef _DEBUG
        printf("[%d] inserido em F1\n",pid);
        #endif
      }
    } /* end parsing */
    return EXIT_SUCCESS;
  }
  
  
  
