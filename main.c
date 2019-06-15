
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

  /*******************/
  /* data structures */
  /*******************/

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

  /********************/
  /* global variables */
  /********************/

  qhead proc_queues[N_OF_QUEUES];
  qhead aux_queue = NULL;
  qnode current_proc;
  procqueue current_queue;

  int processes_count = 0;
  int io_threads = 0;
  int quantum_timer = 0;
  int semId = 0;

  /*************/
  /* functions */
  /*************/

  // utilities
  void dump_process(process procinfo);
  void dump_queues();
  int fatal_error(const char * err_msg_format, ...);
  procpack * pack_current_process(int stay_on_queue);
  int power_of_two(int exp);

  // macro functions
  int create_queues(void);
  void destroy_queues(void);
  int init_interpreter();

  // specific queues management
  int get_higher_priority_queue_id(int id);
  int get_lower_priority_queue_id(int id);
  int get_queue_runs(int id);
  int get_queue_quantum(int id);
  qhead get_queue_from_id(int id);
  qhead get_updated_queue();
  void set_current_queue(int id);

  // handler functions
  void exit_handler(int pid);
  void io_handler(int pid, int stay_on_queue);
  void * io_thread_handler(void * info);
  void signal_handler(int signo);

  // process rays
  int get_next_ray_age(int * rays, int rays_count, int age);
  int get_ray_sum(int * rays, int rays_count);

  /********/
  /* main */
  /********/

  int main(int argc, char ** argv)
  {
    int ret = 0, quantum = 0;
    qhead queue;
    int queue_id;
    pid_t pid;

    /* create queues */
    if((ret=create_queues())!=0) return ret;

    /* initialize semaphore */
    if((semId = sem_create(SEM_KEY))==-1) return EXIT_FAILURE;
    if(sem_init(semId)==-1) return EXIT_FAILURE;

    /* parse args from stdin and build queues */
    if((ret=init_interpreter())!=0) return ret;

    /* initialize scheduler signal handlers */
    signal(SIGUSR1,signal_handler);

    /* start from queue of highest priority */
    set_current_queue(FIRST_QUEUE_ID);

    #ifdef _DEBUG
    printf("======= SCHEDULING %d PROCESS%s...\n",processes_count,processes_count==1?"":"ES");
    #endif

    /* main loop */
    while( 1 )
    {
      if( processes_count <= 0 ) break;

      if( processes_count != io_threads )
      {
        queue = get_updated_queue();
        quantum = get_queue_quantum(current_queue.id);
        #ifdef _DEBUG
        if( qhead_empty(queue) == QUEUE_FALSE )
          printf("The scheduler is dealing with queue #%d.\n",current_queue.id);
        #endif
      }

      /* current process loop */
      while( 1 )
      {
        int flag = 0;
        char * procname;
        process procinfo;
        /////////////////////////////////
        // ENTERS CRITICAL REGION
        // Manipulates current process
        // and current queue
        /////////////////////////////////
        sem_enter_cr(semId);
        /////////////////////////////////
        if( qhead_empty(queue) == QUEUE_FALSE )
        {
          current_proc = qhead_rm(queue);
          procinfo = (process) qnode_getinfo(current_proc);
          pid = procinfo->pid;
          flag = 1;
        }
        /////////////////////////////////
        sem_exit_cr(semId);
        /////////////////////////////////
        // EXITS CRITICAL REGION
        /////////////////////////////////

        if( !flag ) break;
        procname = procinfo->name;
        quantum_timer = 0;

        #ifdef _DEBUG
        printf("Scheduling process %s...\n",procname);
        #endif

        kill(pid,SIGCONT);
        int ray_end = get_next_ray_age(procinfo->rays, procinfo->rays_count, procinfo->age);
        while(  procinfo->age < ray_end && quantum_timer < quantum ); // wait
        kill(pid,SIGSTOP);

        /////////////////////////////////
        // ENTERS CRITICAL REGION
        // Manipulates current process
        /////////////////////////////////
        sem_enter_cr(semId);
        /////////////////////////////////
        if( procinfo->age == ray_end )
        {
          int termination_age = get_ray_sum(procinfo->rays,procinfo->rays_count);
          if( procinfo->age == termination_age )
          {
            printf("Process %s was terminated.\n",procname);
            exit_handler(pid); // TERMINATED
          }
          else
          {
            int stay_on_queue = quantum_timer == quantum;
            printf("Process %s was blocked by I/O\n",procname);
            if(stay_on_queue)
            {
              printf("...and will remain on queue #%d (for queue quantum coinciding with I/O)\n",current_queue.id);
            }
            io_handler(pid, stay_on_queue); // I/O
          }
        }
        else
        {
          // CPU
          int new_queue_id = get_lower_priority_queue_id(current_queue.id);
          #ifdef _DEBUG
          printf("Process %s was interrupted for exceeding queue quantum of %d time units .\n",procname,quantum);
          #endif
          if( new_queue_id == current_queue.id )
          {
            if( qhead_ins(aux_queue,current_proc) != QUEUE_OK )
            {
              fatal_error("Could not insert process %s in auxiliary queue.\n",
              procname);
              exit(0);
            }
            #ifdef _DEBUG
            printf("Process %s will remain in queue #%d.\n", procname,new_queue_id);
            #endif
          }
          else
          {
            if( qhead_ins(get_queue_from_id(new_queue_id),current_proc) != QUEUE_OK )
            {
              fatal_error("Could not insert process %s in queue #%d.\n",
              procname, new_queue_id);
              exit(0);
            }
            #ifdef _DEBUG
            printf("Process %s will migrate from queue #%d to queue #%d\n",
            procname, current_queue.id, new_queue_id);
            #endif
          }
        }
        dump_queues();
        /////////////////////////////////
        sem_exit_cr(semId);
        /////////////////////////////////
        // EXITS CRITICAL REGION
        /////////////////////////////////
      }
      /////////////////////////////////
      // ENTERS CRITICAL REGION
      // Manipulates auxiliary queue
      /////////////////////////////////
      sem_enter_cr(semId);
      /////////////////////////////////
      if( qhead_transfer(aux_queue,queue,QFLAG_TRANSFER_ALL) != QUEUE_OK )
        return fatal_error("An error occurred while managing auxiliary queue\n");
      /////////////////////////////////
      sem_exit_cr(semId);
      /////////////////////////////////
      // EXITS CRITICAL REGION
      /////////////////////////////////
    }

    #ifdef _DEBUG
    printf("======= END OF SCHEDULING\n\n");
    #endif

    /* safely destroying semaphore */
    sem_destroy(semId);

    /* safely freeing queues */
    destroy_queues();

    /* end scheduler */
	  return EXIT_SUCCESS;
  }

  /*****************************/
  /* Functions' implementation */
  /*****************************/

  // Indicates an error through stderr output
  // err_msg_format - format, just like in printf
  // ... - arguments, just like in printf
  // > EXIT_FAILURE
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

  // Optimized binary exponentiation (base 2)
  // exp - exponent
  // > 2^(exp)
  int power_of_two(int exp) { return 1<<exp; }

  // Creates process queues (+ auxiliary)
  // > EXIT_SUCCESS, EXIT_FAILURE
  int create_queues(void)
  {
    if( qhead_create(&aux_queue,-2) != QUEUE_OK )
      return fatal_error("Could not create auxiliary queue.\n");
    for( int i = 0 ; i < N_OF_QUEUES ; i++ )
      if( qhead_create(proc_queues+i,i) != QUEUE_OK )
        return fatal_error("Could not create queue #%d.\n",i);
    return EXIT_SUCCESS;
  }

  // Destroys process queues (+ auxiliary)
  void destroy_queues(void)
  {
    qhead_destroy(&aux_queue);
    for( int i = 0 ; i < N_OF_QUEUES ; i++ )
      qhead_destroy(proc_queues+i);
  }

  // Get queue from ID
  // id - queue ID
  // > queue head
  qhead get_queue_from_id(int id)
  {
    return proc_queues[id%N_OF_QUEUES];
  }

  // Sums all process rays
  // rays - array of ray durations
  // > sum of rays
  int get_ray_sum(int * rays, int rays_count)
  {
    int sum = 0;
    for(int i = 0; i < rays_count; i++) sum += rays[i];
    return sum;
  }

  // Gets nearest age when process will enter I/O
  // rays - array of ray durations
  // rays_count - size of rays array
  // age - process' current age
  // > nearest age
  int get_next_ray_age(int * rays, int rays_count, int age)
  {
    int sum = 0;
    if( age >= get_ray_sum(rays,rays_count) ) return age;
    for(int i = 0; i < rays_count && sum <= age; i++) sum += rays[i];
    return sum;
  }

  // Updates queue
  // > new queue
  qhead get_updated_queue()
  {
    if( current_queue.runs_left == 0 )
    {
      #ifdef _DEBUG
      if( processes_count != io_threads && qhead_empty(current_queue.queue) == QUEUE_FALSE )
        printf("Queue #%d has reached its limit of %d cycles.\n",current_queue.id,get_queue_runs(current_queue.id));
      #endif
      set_current_queue((current_queue.id+1)%N_OF_QUEUES); // forces next queue
    }
    current_queue.runs_left--; // already wastes by calling
    return get_queue_from_id(current_queue.id);
  }

  // Sets current queue from ID
  // id - queue ID
  // [!] invokes semaphore
  void set_current_queue(int id)
  {
    sem_enter_cr(semId);
    current_queue.id = id;
    current_queue.queue = get_queue_from_id(id);
    current_queue.runs_left = get_queue_runs(id);
    sem_exit_cr(semId);
  }

  // Get queue maximum number of runs from ID
  // id - queue ID
  // > 2^(N-id-1), being N the number of queues
  int get_queue_runs(int id) { return power_of_two(N_OF_QUEUES-id-1); }

  // Get queue quantum from ID
  // id - queue ID
  // > q*2^(id), being q the smallest quantum
  int get_queue_quantum(int id) { return power_of_two(id)*SMALLEST_QUANTUM; }

  // Packs current process into a data structure
  // stay_on_queue -  0 = migrates to higher priority queue
  //                  1 = stays on current queue
  // > proccess package
  // [!] needs to be called inside a semaphore
  procpack * pack_current_process(int stay_on_queue)
  {
    int new_queue_id;
    procpack * pack = (procpack *) malloc(sizeof(procpack));
    if( pack == NULL ){
      fatal_error("Could not allocate memory.\n");
      exit(0); //abort program
    }
    pack->process = current_proc;
    if(stay_on_queue) new_queue_id = current_queue.id;
    else new_queue_id = get_higher_priority_queue_id(current_queue.id);
    pack->queue = get_queue_from_id(new_queue_id);
    return pack;
  }

  // Handles IPC signals
  // signo - singal identification
  void signal_handler(int signo)
  {
    if( signo == SIGUSR1 )
    {
      quantum_timer++;
      process procinfo = (process) qnode_getinfo(current_proc);
      procinfo->age = procinfo->age + 1;
    }
  }

  // Destroys current process node and terminates it
  // pid - current process ID
  // [!] needs to be called inside a semaphore
  void exit_handler(int pid)
  {
    qnode dead_node;
    dead_node = current_proc;
    processes_count--;
    qnode_destroy(&dead_node);
    kill(pid,SIGKILL);
  }

  // Blocks the current process, assigning it to a thread
  // The thread will be handled by the io_thread_handler function
  // pid - current process ID
  // stay_on_queue -  0 = migrates to higher priority queue
  //                  1 = stays on current queue
  // [!] needs to be called inside a semaphore
  void io_handler(int pid, int stay_on_queue)
  {
    void * info;
    pthread_t thread;
    info = (void *) pack_current_process(stay_on_queue); // inside semaphore
    pthread_create(&thread,NULL,io_thread_handler,info);
    io_threads++;
    if( io_threads > processes_count ) // Crash-proof
    {
      fatal_error("ERROR: #Threads > #Processes ! ! !\n");
      exit(0);
    }
  }

  // Sleeps for IO_BLOCK_TIME seconds and then inserts on the
  // queue assigned by the process package
  // [!] invokes semaphore
  void * io_thread_handler(void * info)
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
    sleep(IO_BLOCK_TIME); // simulating I/O
    sem_enter_cr(semId);
    if( qhead_getid(new_queue) == current_queue.id )
    {
      new_queue = aux_queue;
    }
    if( qhead_ins(new_queue,io_proc) != QUEUE_OK )
    {
      if( new_queue == aux_queue )
        fatal_error("Could not insert process %s in auxiliary queue.\n",
        procname);
      else
        fatal_error("Could not insert process %s in queue #%d.\n",
        procname, qhead_getid(new_queue));
    }
    io_threads--;
    printf("Process %s is no longer blocked by I/O and was inserted in queue #%d.\n",
    procname, qhead_getid(new_queue));
    sem_exit_cr(semId);
    pthread_exit(NULL); // end thread
  }

  // Gets queue with slightly higher priority
  // id - queue ID
  // > queue with higher priority
  int get_higher_priority_queue_id(int id)
  {
    if(id <= 0) return 0;
    return (id-1)%N_OF_QUEUES;
  }

  // Gets queue with slightly lower priority
  // id - queue ID
  // > queue with lower priority
  int get_lower_priority_queue_id(int id)
  {
    if(id >= N_OF_QUEUES-1) return N_OF_QUEUES-1;
    return (id+1)%N_OF_QUEUES;
  }

  // Dumps queues to stdout output, calling dump_process for each
  // process it finds in queue. Also dumps the number of processes
  // in total, in I/O and in queues.
  // [!] needs to be inside a semaphore!!
  void dump_queues()
  {
    for(int i = 0 ; i < N_OF_QUEUES ; i++ )
    {
      qhead f = proc_queues[i];
      qhead dump_queue;
      int isEmpty = qhead_empty(f) == QUEUE_OK;
      if( i == current_queue.id ) isEmpty &= qhead_empty(aux_queue)==QUEUE_OK;
      printf("Queue #%d: %s\n",i,isEmpty?
      "empty":"not empty (from first to last)");
      qhead_create(&dump_queue,-2);
      qnode node;
      while( (node = qhead_rm(f)) != NULL )
      {
        process procinfo = (process) qnode_getinfo(node);
        if( qhead_ins(dump_queue,node) != QUEUE_OK )
        {
          fatal_error("Could not insert process %s in dumping queue.\n",
          procinfo->name);
          exit(0);
        }
        printf("- ");
        dump_process(procinfo);
      }
      qhead_transfer(dump_queue,f,QFLAG_TRANSFER_ALL);
      if( i == current_queue.id )
      {
        while( (node = qhead_rm(aux_queue)) != NULL )
        {
          process procinfo = (process) qnode_getinfo(node);
          if( qhead_ins(dump_queue,node) != QUEUE_OK )
          {
            fatal_error("Could not insert process %s in dumping queue.\n",
            procinfo->name);
            exit(0);
          }
          printf("* ");
          dump_process(procinfo);
        }
        qhead_transfer(dump_queue,aux_queue,QFLAG_TRANSFER_ALL);
      }
      qhead_destroy(&dump_queue);
    }
    if( processes_count > 0 )
    {
      int one = processes_count == 1;
      printf("There %s %d remaining process%s\n",one?"is":"are",processes_count,
              one?"":"es");
      printf("* %d in queue\n",processes_count-io_threads);
      printf("* %d blocked by I/O\n",io_threads);
    }
    else
      printf("No remaining processes\n");
  }

  // Dumps process information to stdout
  // procinfo - process information
  void dump_process(process procinfo)
  {
    int ray_sum = get_ray_sum(procinfo->rays,procinfo->rays_count);
    int next_io = get_next_ray_age(procinfo->rays,procinfo->rays_count,procinfo->age) - procinfo->age;
    printf("%s [%d] %d/%d, %d left for next I/O\n",
            procinfo->name,
            procinfo->pid,
            procinfo->age,
            ray_sum,
            next_io);
  }

  // Initializes interpreter that will read from stdin the
  // set of commands to create the processes that will go
  // to the queue of id equal to FIRST_QUEUE_ID
  // > EXIT_SUCCESS, EXIT_FAILURE
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
        if( qnode_create(&process_node,process_info) != QUEUE_OK )
        {
          return fatal_error("Could not allocate memory.\n");
        }
        if( qhead_ins(proc_queues[FIRST_QUEUE_ID],process_node) != QUEUE_OK )
        {
          return fatal_error("Could not insert process %s in queue #%d.\n",
          prog, FIRST_QUEUE_ID);
        }
        processes_count++;
        #ifdef _DEBUG
        printf("Child %s inserted in queue #%d\n", prog, FIRST_QUEUE_ID);
        #endif
      }
    } /* end parsing */

    #ifdef _DEBUG
      printf("======= PARSING ENDED...\n\n");
    #endif
    return EXIT_SUCCESS;
  }
