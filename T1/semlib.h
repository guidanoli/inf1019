
  // Semaphore Library
  // courtesy of L.F. Seibel
  // http://139.82.24.35/seibel/hp/

  #ifndef _H_SEMLIB
  #define _H_SEMLIB

  // Create sempahore
  // key - unique integer
  // > semId, -1 (err)
  int sem_create(int key);

  // Initalize semaphore
  // semId - sem_create return
  // > 0, -1 (err)
  int sem_init(int semId);

  // Destroy semaphore
  // semId - sem_create return
  void sem_destroy(int semId);

  // Enter critic region
  // semId - sem_create return
  int sem_enter_cr(int semId);

  // Exit critic region
  // semId - sem_create return
  int sem_exit_cr(int semId);

  ////////////////////////////////

  #endif
