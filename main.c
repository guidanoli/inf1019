
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
  
  #define FIRST_QUEUE_ID 0
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
  int processes_count = 0;
  int io_threads = 0;
  
  // semaphore and queues
  int semId = 0;
  qhead signal_queue;
  
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
  void dummy_handler(int signo);
  void ioHandler(int pid);
  void signalHandler(int signo);
  void * ioThreadFunction(void * info);
  procpack * getCurrentProcessPackage();
  void dump_queues();

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
   
    /* initialize interpreter signal handler */
    signal(SIGUSR2,dummy_handler);
    
    /* parse args from stdin and build queues */
    if((ret=init_interpreter())!=0) return ret;
    
    /* initialize scheduler signal handlers */
    signal(SIGUSR1,signalHandler);
    signal(SIGUSR2,signalHandler);
            
    /* start from queue of highest priority */
    setCurrentQueue(FIRST_QUEUE_ID);
    
    #ifdef _DEBUG
    printf("Há %d processos na fila de processos prontos para serem executados.\n",processes_count);
    printf("Iniciando escalonamento...\n");
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
      #ifdef _DEBUG
      if( qhead_empty(queue) == QUEUE_FALSE )
        printf("Escalonador está manipulando a fila #%d.\n",current_queue.id);
      #endif
      
      /* current process loop */
      while( 1 )
      {
        int flag = 0;
        /////////////////////////////////
        // ENTERS CRITICAL REGION
        // Manipulates current process
        // and current queue
        /////////////////////////////////
        enterCR(semId);
        /////////////////////////////////
        if( qhead_empty(queue) == QUEUE_FALSE )
        {
          current_proc.node = qhead_rm(queue);
          current_proc.status = NORMAL;
          pid = qnode_getid(current_proc.node);
          flag = 1;
        }
        /////////////////////////////////
        exitCR(semId);
        /////////////////////////////////
        // EXITS CRITICAL REGION
        /////////////////////////////////        
        if( !flag ) break;
        
        #ifdef _DEBUG
        printf("Escalonando processo %d...\n",pid);
        #endif
        
        kill(pid,SIGCONT);
        sleep(quantum); // z z z ...
        kill(pid,SIGSTOP);
        
        #ifdef _DEBUG
        printf("Processo %d interrompido.\n",pid);
        #endif
        
        /////////////////////////////////
        // ENTERS CRITICAL REGION
        // All signals have to be treated
        /////////////////////////////////
        enterCR(semId);
        /////////////////////////////////
        while( qhead_empty(signal_queue) == QUEUE_FALSE )
        {
          qnode sig;
          int signo;
          sig = qhead_rm(signal_queue);
          signo = qnode_getid(sig);
          qnode_destroy(&sig);
          if( signo == SIGUSR1 )
          {
            ioHandler(pid);
            current_proc.status = IO;
          }
          if( signo == SIGUSR2 )
          {
            exitHandler(pid);
            current_proc.status = TERMINATED;
          }
        }
        /////////////////////////////////
        exitCR(semId);
        /////////////////////////////////
        // EXITS CRITICAL REGION
        /////////////////////////////////
        
        /////////////////////////////////
        // ENTERS CRITICAL REGION
        // Manipulates current process
        /////////////////////////////////
        enterCR(semId);
        /////////////////////////////////
        if( current_proc.status == NORMAL )
        {
          int new_queue_id = getLowerPriorityQueueId(current_queue.id);
          #ifdef _DEBUG
          printf("Processo %d excedeu o quantum da fila igual a %d u.t. .\n",pid,quantum);
          #endif
          if( new_queue_id == current_queue.id )
          {
            qhead_ins(aux_queue,current_proc.node);
            #ifdef _DEBUG
            printf("Processo %d continuará na fila #%d.\n", pid,new_queue_id);
            #endif
          }
          else
          {
            qhead_ins(getQueueFromId(new_queue_id),current_proc.node);
            #ifdef _DEBUG
            printf("Processo %d migrará da fila #%d para fila #%d\n", pid, current_queue.id, new_queue_id);
            #endif
          }
        }
        /////////////////////////////////
        exitCR(semId);
        /////////////////////////////////
        // EXITS CRITICAL REGION
        /////////////////////////////////
      }
      /////////////////////////////////
      // ENTERS CRITICAL REGION
      // Manipulates auxiliary queue
      /////////////////////////////////
      enterCR(semId);
      /////////////////////////////////
      if( qhead_transfer(aux_queue,queue,QFLAG_TRANSFER_ALL) != QUEUE_OK )
        return fatal_error("Erro ao administrar filas auxiliares\n");
      /////////////////////////////////
      exitCR(semId);
      /////////////////////////////////
      // EXITS CRITICAL REGION
      /////////////////////////////////
    }
    
    #ifdef _DEBUG
    printf("Fim do escalonamento. Todos os processos terminados.\n");
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
    if( qhead_create(&signal_queue,-1) != 0 )
      return fatal_error("Falha ao criar fila de sinais.\n");
    if( qhead_create(&aux_queue,-1) != 0 )
      return fatal_error("Falha ao criar fila auxiliar.\n");
    for( int i = 0 ; i < N_OF_QUEUES ; i++ )
      if( qhead_create(proc_queues+i,i) != 0 )
        return fatal_error("Falha ao crier fila #%d.\n",i);
    return 0;
  }

  void destroy_queues(void)
  {
    qhead_destroy(&signal_queue);
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
    if( current_queue.runs_left == 0 )
    {
      forceNextQueue();
    }
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
    
  // needs to be called inside a semaphore
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
  
  // Adds signal to signal queue
  void signalHandler(int signo)
  {    
    qnode sig;
    enterCR(semId); // enters CR
    qnode_create(&sig,signo);
    qhead_ins(signal_queue,sig);
    exitCR(semId); // exits CR
  }
  
  // needs to be called inside a semaphore
  void ioHandler(int pid)
  {
    void * info;
    pthread_t thread;
    info = (void *) getCurrentProcessPackage(); // inside semaphore
    pthread_create(&thread,NULL,ioThreadFunction,info);
    io_threads++;
    #ifdef _DEBUG
    if( io_threads > processes_count ) // Crash-proof
    {
      printf("#Threads > #Processes ! ! !\n");
      exit(0);
    }
    #endif
  }
  
  // needs to be called inside a semaphore
  void exitHandler(int pid)
  {
    qnode dead_node;
    dead_node = current_proc.node;
    processes_count--;
    qnode_destroy(&dead_node);
    kill(pid,SIGKILL);
    printf("Processo %d terminou.\n",pid);
    #ifdef _DEBUG
    dump_queues();
    #endif
    if( processes_count > 0 )
    {
      printf("Há %d processos restantes\n",processes_count);
      printf("* %d em fila\n",processes_count-io_threads);
      printf("* %d em IO\n",io_threads);
    }
    else
      printf("Não há processos restantes\n");
    
  }
  
  void * ioThreadFunction(void * info)
  {
    procpack * pack;
    qnode io_proc;
    qhead new_queue;
    int my_pid;
    pack = (procpack *) info;
    io_proc = pack->process;
    my_pid = qnode_getid(io_proc);
    new_queue = pack->queue;
    //free(info);
    printf("Processo %d está em IO.\n",my_pid);
    sleep(IO_BLOCK_TIME); // simulating IO
    /////////////////////////////////
    // ENTERS CRITICAL REGION
    // Manipulates io_process and
    // any of the process queues
    /////////////////////////////////
    enterCR(semId);
    /////////////////////////////////
    qhead_ins(new_queue,io_proc);
    io_threads--;
    printf("Processo %d terminou IO.\n",my_pid);
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
   
  void dummy_handler(int signo) {}
  
  // needs to be inside a semaphore!!
  void dump_queues()
  {
    for(int i = 0 ; i < N_OF_QUEUES ; i++ )
    {
      qhead f = proc_queues[i];
      qhead aux;
      printf("Fila #%d: %s\n",i,(qhead_empty(f)==QUEUE_OK)?"vazia":"não vazia");
      qhead_create(&aux,-1);
      while( qhead_empty(f) == QUEUE_FALSE )
      {
        qnode n = qhead_rm(f);
        qhead_ins(aux,n);
        printf("- %d\n",qnode_getid(n));
      }
      qhead_transfer(aux,f,QFLAG_TRANSFER_ALL);
      qhead_destroy(&aux);
    }
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
        char buffer[QTD_ARGS*(ARG_SIZE+1)+1] = "";
        for( int i = 0 ; i < qt_raj+2 ; i++ )
        {
          args[i] = (char *) malloc(sizeof(char)*ARG_SIZE);
          if( i == 0 ) strcpy(args[0],prog);
          else if( i < qt_raj+1 ) sprintf(args[i],"%d",raj[i-1]);
          if( i < qt_raj+1 )
          {
            strcat(buffer,args[i]);
            strcat(buffer," ");
          }
        }
        args[qt_raj+1] = NULL;
        printf("Processo filho %d executando... %s\n",getpid(),buffer);
        if( execv(*args,args) == -1 )
        {
          return fatal_error("Não foi possível executar programa %d.\n",getpid());
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
        qhead_ins(proc_queues[FIRST_QUEUE_ID],process);
        sleep(1);
        processes_count++;
        #ifdef _DEBUG
        printf("Processo filho %d inserido na fila #%d\n",pid,FIRST_QUEUE_ID);
        #endif
      }
    } /* end parsing */
    return EXIT_SUCCESS;
  }
  
  
  
