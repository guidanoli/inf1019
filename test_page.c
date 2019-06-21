
  // Page test
  // Guilherme Dantas

  #include <stdio.h>
  #include <string.h>
  #include <assert.h>
  #include "page.h"

  /* TEST MODULE TOOLKIT */

  #define DEFAULT_COLOR "\033[0m"
  #define RED "\033[0;31m"
  #define GREEN "\033[0;32m"
  #define YELLOW "\033[0;33m"
  #define assert(x); assertcolor(x,#x,__LINE__);
  #define fatal_assert(x); abort_test(x,#x,__LINE__);

  static void abort_test(int boolean, const char * label, const line);
  static void assertcolor(int boolean, const char * label, const line);
  static void printcolor(const char * tag, const char * color, const char * msg);
  static void printerror(const char * msg);
  static void printmsg(const char * msg);
  static void printsuccess(const char * msg);
  static void show_log();

  /* CONSTANTS */

  int n_tests = 0;
  int n_failed = 0;

  int main( void )
  {
    page pg = 0;

    // Initialized values are all zero
    assert(page_get_time(pg)==0);
    assert(!page_get_mflag(pg));
    assert(!page_get_rflag(pg));
    dump_page(pg);

    // Time
    int time;
    for(time = 0; time < 8; time++)
    {
      page_set_time(&pg,time);
      assert(page_get_time(pg)==time);
    }
    dump_page(pg);

    int overflow_time = ~((unsigned int)0);
    page_set_time(&pg,overflow_time);
    assert(page_get_time(pg)!=overflow_time);
    dump_page(pg);

    time = 0x3FFFFFFF; // limit
    page_set_time(&pg,time);
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Modified flag
    page_set_mflag(&pg,1);
    assert(page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Referenced flag
    page_set_rflag(&pg,1);
    assert(page_get_rflag(pg));
    assert(page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Setting modified flags back to 0
    page_set_mflag(&pg,0);
    assert(!page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Setting referenced flags back to 0
    page_set_rflag(&pg,0);
    assert(!page_get_rflag(pg));
    assert(!page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Try to overflow referenced flag
    page_set_rflag(&pg,2);
    assert(page_get_rflag(pg));
    assert(!page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Try to overflow modified flag
    page_set_mflag(&pg,2);
    assert(page_get_rflag(pg));
    assert(page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // show log
    show_log();

    return 0;
  }

  static void abort_test(int boolean, const char * label, const line)
  {
    if(!boolean)
    {
      assertcolor(boolean,label,line);
      printerror("The program will be aborted due to a fatal error.");
      show_log();
      exit(1);
    }
  }

  static void show_log()
  {
    char msg[256];
    sprintf(msg,"%d asserts",n_tests);
    printcolor("LOG",YELLOW,msg);
    if( n_failed == 0 ) sprintf(msg,"No errors found");
    else sprintf(msg,"%d error%s found",n_failed,n_failed==1?"":"s");
    printcolor("LOG",YELLOW,msg);
  }

  static void printcolor(const char * tag, const char * color, const char * msg)
  {
    printf(DEFAULT_COLOR);  printf("[");
    printf(color);          printf(tag);
    printf(DEFAULT_COLOR);  printf("] %s\n",msg);
  }

  static void printerror(const char * msg)
  {
    printcolor("ERROR",RED,msg);
    n_failed++;
  }

  static void printmsg(const char * msg)
  {
    printcolor("MSG",DEFAULT_COLOR,msg);
  }

  static void printsuccess(const char * msg)
  {
    printcolor("SUCCESS",GREEN,msg);
  }

  static void assertcolor(int boolean, const char * label, const line)
  {
    if(boolean)
    {
      printsuccess(label);
    }
    else
    {
      char msg[256] = "";
      char line_s[8] = "";
      strcpy(msg,label);
      strcat(msg," failed at line ");
      sprintf(line_s,"%d.",line);
      strcat(msg,line_s);
      printerror(msg);
    }
    n_tests++;
  }
