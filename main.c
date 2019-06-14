
  // Interpreter & Scheduler
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
  #define USER_PROGRAM_NAME "prog"

  struct process_struct {
    char * name;
    int * rays;
    int pid;
    int rays_count;
    int age;
  };

  typedef struct process_struct * process;

  typedef struct procqueue_s {
    qhead queue;
    int id;
    int runs_left;
  } procqueue;

  typedef struct procpack_s {
    qnode process;
    qhead queue;
  } procpack;

  // processes and queues
  qhead proc_queues[N_OF_QUEUES];
  qhead aux_queue = NULL;
  qnode current_proc;
  procqueue current_queue;
  int processes_count = 0;
  int io_threads = 0;
  int quantum_timer = 0;

  // semaphore and queues
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
  void ioHandler(int pid);
  void signalHandler(int signo);
  void * ioThreadFunction(void * info);
  procpack * getCurrentProcessPackage();
  void dump_queues();
  int getRaySum(int * rays, int rays_count);
  int getNextRayAge(int * rays, int rays_count, int age);
  void dump_process(process procinfo);

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

    /* parse args from stdin and build queues */
    if((ret=init_interpreter())!=0) return ret;

    /* initialize scheduler signal handlers */
    signal(SIGUSR1,signalHandler);

    /* start from queue of highest priority */
    setCurrentQueue(FIRST_QUEUE_ID);

    #ifdef _DEBUG
    printf("======= SCHEDULING %d PROCESS%s...\n",processes_count,processes_count==1?"":"ES");
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

      if( processes_count != io_threads )
      {
        queue = getUpdatedQueue();
        quantum = getQueueQuantum(current_queue.id);
        #ifdef _DEBUG
        if( qhead_empty(queue) == QUEUE_FALSE )
          printf("The scheduler is dealing with queue #%d.\n",current_queue.id);
        #endif
      }

      /* current process loop */
      while( 1 )
      {
        int flag = 0;
        int quantum_timer = 0;
        char * procname;
        process procinfo;
        /////////////////////////////////
        // ENTERS CRITICAL REGION
        // Manipulates current process
        // and current queue
        /////////////////////////////////
        enterCR(semId);
        /////////////////////////////////
        if( qhead_empty(queue) == QUEUE_FALSE )
        {
          current_proc = qhead_rm(queue);
          procinfo = (process) qnode_getinfo(current_proc);
          pid = procinfo->pid;
          flag = 1;
        }
        /////////////////////////////////
        exitCR(semId);
        /////////////////////////////////
        // EXITS CRITICAL REGION
        /////////////////////////////////
        if( !flag ) break;
        procname = procinfo->name;

        #ifdef _DEBUG
        printf("Scheduling process %s...\n",procname);
        #endif

        kill(pid,SIGCONT);
        int ray_end = getNextRayAge(procinfo->rays, procinfo->rays_count, procinfo->age);
        while(  procinfo->age < ray_end
                && quantum_timer < quantum ); // wait
        kill(pid,SIGSTOP);

        /////////////////////////////////
        // ENTERS CRITICAL REGION
        // Manipulates current process
        /////////////////////////////////
        enterCR(semId);
        /////////////////////////////////
        if( quantum_timer <= quantum )
        {
          int termination_age = getRaySum(procinfo->rays,procinfo->rays_count);
          if( procinfo->age == termination_age )
          {
            // TERMINATED
            printf("Process %s was terminated.\n",procname);
            exitHandler(pid);
          }
          else
          {
            // IO
            ioHandler(pid);
          }
        }
        else
        {
          int new_queue_id = getLowerPriorityQueueId(current_queue.id);
          #ifdef _DEBUG
          printf("Process %s was interrupted for exceeding queue quantum of %d time units .\n",procname,quantum);
          #endif
          if( new_queue_id == current_queue.id )
          {
            qhead_ins(aux_queue,current_proc);
            #ifdef _DEBUG
            printf("Process %s will remain in queue #%d.\n", procname,new_queue_id);
            #endif
          }
          else
          {
            qhead_ins(getQueueFromId(new_queue_id),current_proc);
            #ifdef _DEBUG
            printf("Process %s will migrate from queue #%d to queue #%d\n",
            procname, current_queue.id, new_queue_id);
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
        return fatal_error("An error occurred while managing auxiliary queue\n");
      /////////////////////////////////
      exitCR(semId);
      /////////////////////////////////
      // EXITS CRITICAL REGION
      /////////////////////////////////
    }

    #ifdef _DEBUG
    printf("======= END OF SCHEDULING\n\n");
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
    printf("\033[0;31m"); // TEXT IS NOW RED
    vfprintf(stderr,err_msg_format,vl);
    printf("\033[0m"); // text is now back to default color
    va_end(vl);
    return EXIT_FAILURE;
  }

  int create_queues(void)
  {
    if( qhead_create(&aux_queue,-1) != 0 )
      return fatal_error("Could not create auxiliary queue.\n");
    for( int i = 0 ; i < N_OF_QUEUES ; i++ )
      if( qhead_create(proc_queues+i,i) != 0 )
        return fatal_error("Could not create queue #%d.\n",i);
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

  int getRaySum(int * rays, int rays_count)
  {
    int sum = 0;
    for(int i = 0; i < rays_count; i++) sum += rays[i];
    return sum;
  }

  int getNextRayAge(int * rays, int rays_count, int age)
  {
    int sum = 0;
    if( age >= getRaySum(rays,rays_count) ) return age;
    for(int i = 0; i < rays_count && sum <= age; i++) sum += rays[i];
    return sum;
  }

  qhead getUpdatedQueue()
  {
    if( current_queue.runs_left == 0 )
    {
      #ifdef _DEBUG
      if( processes_count != io_threads && qhead_empty(current_queue.queue) == QUEUE_FALSE )
        printf("Queue #%d has reached its limit of %d cycles.\n",current_queue.id,getQueueRuns(current_queue.id));
      #endif
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
      fatal_error("Could not allocate memory.\n");
      exit(0); //abort program
    }
    pack->process = current_proc;
    pack->queue = getQueueFromId(getHigherPriorityQueueId(current_queue.id));
    return pack;
  }

  // Adds signal to signal queue
  void signalHandler(int signo)
  {
    // new
    if( signo == SIGUSR1 )
    {
      quantum_timer++;
      process procinfo = (process) qnode_getinfo(current_proc);
      procinfo->age = procinfo->age + 1;
    }
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
      fatal_error("ERROR: #Threads > #Processes ! ! !\n");
      exit(0);
    }
    #endif
  }

  // needs to be called inside a semaphore
  void exitHandler(int pid)
  {
    qnode dead_node;
    dead_node = current_proc;
    processes_count--;
    qnode_destroy(&dead_node);
    kill(pid,SIGKILL);
    #ifdef _DEBUG
    dump_queues();
    #endif
    if( processes_count > 0 )
    {
      printf("There are %d remaining processes\n",processes_count);
      printf("* %d in queue\n",processes_count-io_threads);
      printf("* %d blocked by IO\n",io_threads);
    }
    else
      printf("No remaining processes\n");

  }

  void * ioThreadFunction(void * info)
  {
    procpack * pack;
    qnode io_proc;
    qhead new_queue;
    int my_pid;
    char * procname;
    pack = (procpack *) info;
    io_proc = pack->process;
    process procinfo = (process) qnode_getinfo(io_proc);
    procname = procinfo->name;
    my_pid = procinfo->pid;
    new_queue = pack->queue;
    printf("Process %s was blocked by IO.\n",procname);
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
    printf("Process %s is no longer blocked by IO.\n",procname);
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

  // needs to be inside a semaphore!!
  void dump_queues()
  {
    for(int i = 0 ; i < N_OF_QUEUES ; i++ )
    {
      qhead f = proc_queues[i];
      qhead aux;
      printf("Queue #%d: %s\n",i,(qhead_empty(f)==QUEUE_OK)?
      "empty":"not empty (from first to last)");
      qhead_create(&aux,-1);
      while( qhead_empty(f) == QUEUE_FALSE )
      {
        qnode n = qhead_rm(f);
        qhead_ins(aux,n);
        process procinfo = (process) qnode_getinfo(n);
        printf("- ");
        dump_process(procinfo);
      }
      qhead_transfer(aux,f,QFLAG_TRANSFER_ALL);
      qhead_destroy(&aux);
    }
  }

  void dump_process(process procinfo)
  {
    int ray_sum = getRaySum(procinfo->rays,procinfo->rays_count);
    int next_io = getNextRayAge(procinfo->rays,procinfo->rays_count,procinfo->age) - procinfo->age;
    printf("%s [%d] %d/%d, %d left for next I/O\n",
            procinfo->name,
            procinfo->pid,
            procinfo->age,
            ray_sum,
            next_io);
  }

  int init_interpreter()
  {
    char c;
    pid_t pid;
    #ifdef _DEBUG
      printf("\n======= PARSING...\n");
    #endif
    /* parse args from stdin */
    while((c = fgetc(stdin)) == 'e')
    {
      char prog[ARG_SIZE] = "";
      int raj[QTD_ARGS];
      int qt_raj = 1;
      qnode process_node;
      process process_info;

      if( scanf("xec %s (%d",prog,raj) != 2 )
      {
        return fatal_error("Bad string format.\n");
      }
      while((c = fgetc(stdin)) == ',')
      {
        if( scanf("%d",raj+qt_raj) != 1 )
        {
          return fatal_error("Bad string format.\n");
        }
        qt_raj++;
      }
      if( c != ')' )
      {
        return fatal_error("Bad string format.\n");
      }
      scanf(" ");
      if((pid = fork()) == 0)
      {
        char * args[QTD_ARGS];
        char temp[ARG_SIZE+1];
        char buffer[QTD_ARGS*(ARG_SIZE)+1] = "";
        for( int i = 0 ; i < qt_raj+1 ; i++ )
        {
          if( i == 0 ) strcpy(temp,USER_PROGRAM_NAME);
          else sprintf(temp,"%d",raj[i-1]);

          args[i] = strdup(temp);

          if( i != 0 )
          {
            strcat(buffer,args[i]);
            strcat(buffer," ");
          }
        }
        args[qt_raj+1] = NULL;
        printf("Child process %s is executing... ( %s)\n",prog,buffer);
        if( execv(*args,args) == -1 )
        {
          return fatal_error("Could not execute process %s.\n",prog);
        }
        exit(0);
      }
      else
      {
        kill(pid,SIGSTOP);
        process_info = (process) malloc(sizeof(struct process_struct));
        if( process_info == NULL )
        {
          return fatal_error("Could not allocate struct.\n");
        }
        process_info->pid = pid;
        process_info->name = strdup(prog);
        process_info->age = 0;
        process_info->rays_count = qt_raj;
        process_info->rays = (int *) malloc(sizeof(int)*qt_raj);
        for(int i = 0 ; i < qt_raj ; i++)
        {
          process_info->rays[i] = raj[i];
        }
        dump_process(process_info);
        if( qnode_create(&process_node,process_info) != 0 )
        {
          fprintf(stderr,"Could not allocate memmory.\n");
          return EXIT_FAILURE;
        }
        qhead_ins(proc_queues[FIRST_QUEUE_ID],process_node);
        sleep(1);
        processes_count++;
        #ifdef _DEBUG
        printf("Child %s inserted in queue #%d\n",prog,FIRST_QUEUE_ID);
        #endif
      }
    } /* end parsing */

    #ifdef _DEBUG
      printf("======= PARSING ENDED...\n\n");
    #endif
    return EXIT_SUCCESS;
  }
