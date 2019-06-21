
  // Utilities module implementation
  // Guilherme Dantas

  #include <stdio.h>
  #include <stdlib.h>
  #include <stdarg.h>
  #include "utils.h"

  /*********************/
  /* Private functions */
  /*********************/

  static void print_bin_aux(int n);

  /**********************/
  /* Exported functions */
  /**********************/

  int fatal_error (const char * err_msg_format, ...)
  {
    va_list vl;
    va_start(vl, err_msg_format);
    fprintf(stderr, "\033[1;31m");
    vfprintf(stderr,err_msg_format, vl);
    fprintf(stderr, "\033[0m");
    va_end(vl);
    return EXIT_FAILURE;
  }

  int is_power_of_two (int n)
  {
    if( n <= 0 ) return 0;
    while( (n & 1) == 0 ) n >>= 1;
    return n == 1;
  }

  void print_bin (int n)
  {
    print_bin_aux(n);
    if( n == 0 ) printf("0");
    printf("\n");
  }

  /************************************/
  /* Private functions implementation */
  /************************************/

  static void print_bin_aux(int n)
  {
    if( n == 0 ) return;
    print_bin_aux(n>>1);
    printf("%d",n&1);
  }
