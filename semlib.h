  
  #ifndef _H_SEMAPHORE_LIBRARY
  #define _H_SEMAPHORE_LIBRARY
   
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
  
  // creates sempahore
  // key must be unique
  // returns semId
  int semCreate(int key);
  // initalize semaphore
  // returns -1 on error
  int semInit(int semId);
  // destroy semaphore
  void semDestroy(int semId);
  // enter critic region
  int enterCR(int semId);
  // exit critic region
  int exitCR(int semId);
    
  ////////////////////////////////
  
  #endif
