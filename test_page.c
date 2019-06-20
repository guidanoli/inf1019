
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
    sprintf(msg,"%d error%s found",n_failed,n_failed==0?"":"s");
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
