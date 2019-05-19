  
  ////////////////////////////////////
  //                                //
  // SEMAPHORES - Do Not Touch !!!  //
  //                                //
  ////////////////////////////////////
  //                                //
  // courtesy of L.F. Seibel        //
  // http://139.82.24.35/seibel/hp/ //
  //                                //
  ////////////////////////////////////
  
  #include <sys/sem.h>
  #include "semlib.h"
  
  union semun
  {
	  int val;
	  struct semid_ds *buf;
	  ushort *array;
  };
  int semCreate(int key)
  {
    return semget(key, 1, 0666 | IPC_CREAT);
  }
  int semInit(int semId)
  {
    union semun semUnion;
    semUnion.val = 1;
    return semctl(semId, 0, SETVAL, semUnion);
  }
  void semDestroy(int semId)
  {
    union semun semUnion;
    semctl(semId, 0, IPC_RMID, semUnion);
  }
  int enterCR(int semId)
  {
    struct sembuf semB;
    semB.sem_num = 0;
    semB.sem_op = -1;
    semB.sem_flg = SEM_UNDO;
    semop(semId, &semB, 1);
    return 0;
  }
  int exitCR(int semId)
  {
    struct sembuf semB;
    semB.sem_num = 0;
    semB.sem_op = 1;
    semB.sem_flg = SEM_UNDO;
    semop(semId, &semB, 1);
    return 0;
  }
  
  //////////////////////////////
