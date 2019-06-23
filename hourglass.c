
  // Hourglass module implementation
  // Guilherme Dantas

  #include <time.h>
  #include "hourglass.h"

  static clock_t start = -1;
  static clock_t end = -1;
  static double elapsed_time = 0;
  static char state = 0;

  static void update_elapsed_time();

  void hourglass_begin ()
  {
    if( state == 1 ) return;
    start = clock();
    state = 1;
  }

  void hourglass_stop ()
  {
    if( state == 0 ) return;
    end = clock();
    update_elapsed_time();
    state = 0;
  }

  int hourglass_seconds ()
  {
    return ((int) elapsed_time) % 60;
  }

  int hourglass_minutes()
  {
    return (int) (elapsed_time / 60);
  }

  static void update_elapsed_time()
  {
    elapsed_time = ((double) end - (double) start) / CLOCKS_PER_SEC;
  }
