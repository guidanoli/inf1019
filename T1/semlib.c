
  // Semaphore Library
  // courtesy of L.F. Seibel
  // http://139.82.24.35/seibel/hp/

  #include <sys/sem.h>
  #include "semlib.h"

  union semun
  {
	  int val;
	  struct semid_ds *buf;
	  ushort *array;
  };

  int sem_create(int key)
  {
    return semget(key, 1, 0666 | IPC_CREAT);
  }

  int sem_init(int semId)
  {
    union semun semUnion;
    semUnion.val = 1;
    return semctl(semId, 0, SETVAL, semUnion);
  }

  void sem_destroy(int semId)
  {
    union semun semUnion;
    semctl(semId, 0, IPC_RMID, semUnion);
  }

  int sem_enter_cr(int semId)
  {
    struct sembuf semB;
    semB.sem_num = 0;
    semB.sem_op = -1;
    semB.sem_flg = SEM_UNDO;
    return semop(semId, &semB, 1);
  }

  int sem_exit_cr(int semId)
  {
    struct sembuf semB;
    semB.sem_num = 0;
    semB.sem_op = 1;
    semB.sem_flg = SEM_UNDO;
    return semop(semId, &semB, 1);
  }
