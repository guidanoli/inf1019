
  // Test module interface
  // Guilherme Dantas

  #ifndef TEST_H
  #define TEST_H

  #include "colours.h"

  // Aborts test, displaying the statement that caused it and line,
  // and, additionally, shows log.
  // boolean - if false, error is thrown
  // label - statement string
  // line - statement line in code
  // > (to stderr) error message + test log, nothing
  // [!] if boolean is false, the program is exited.
  #define fatal_assert(statement); abort_test(statement,#statement,__LINE__);
  void abort_test(int boolean, const char * label, const line);

  // Asserts statement, displaying it and the line it's at if false
  // boolean - if false, error is thrown
  // label - statement string
  // line - statement line in code
  // > (to stderr) error message, nothing
  #define assert(statement); assertcolor(statement,#statement,__LINE__);
  void assertcolor(int boolean, const char * label, const line);

  // Prints message to screen properly formatted
  // tag - message header that will appear in between brackets
  // color - see colours.h
  // msg, ... - format string and parameters like printf
  // > (to stdout) message
  void printmsg(const char * tag, const char * color, const char * msg, ...);

  // Ouputs information stored in counters at a given time
  // > (to stdout) test log
  void show_log();

  #endif
