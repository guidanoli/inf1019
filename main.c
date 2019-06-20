
  // Main Module
  // Guilherme Dantas

  #include <stdio.h>
  #include <stdlib.h>
  #include <stdarg.h>
  #include <string.h>
  #include <unistd.h>

  #define PARAMS "<program> <algorithm> <log path> <page size> <total size>"

  typedef enum {
    LRU,
    NRU,
    NOVO
  } algorithm_t;

  /********************/
  /* Static functions */
  /********************/

  static int fatal_error(const char * err_msg_format, ...);

  /********************/
  /* Global variables */
  /********************/

  int page_size; // in KB
  int total_size; // in MB

  /********/
  /* Main */
  /********/

  int main (int argc, char ** argv)
  {
    algorithm_t algorithm;

    // Arguments validation
    if( argc != 5 ) return fatal_error("Invalid parameters.\nExpected: %s\n", PARAMS);
    if( !strcmp(argv[1],"LRU") ) algorithm = LRU;
    else if( !strcmp(argv[1],"NRU") ) algorithm = NRU;
    else if( !strcmp(argv[1],"NOVO") ) algorithm = NOVO;
    else return fatal_error("Invalid algorithm.\nValid values: LRU, NRU, NOVO\n");
    if( access(argv[2],F_OK) == -1 ) return fatal_error("File %s does not exist.\n",argv[2]);
    if( (page_size = atoi(argv[3])) <= 0 ) return fatal_error("Invalid page size\n");
    if( page_size < 8 || page_size > 32 ) return fatal_error("Page size must be between 8 and 32 KB.\n");
    if( (total_size = atoi(argv[4])) <= 0 ) return fatal_error("Invalid total size\n");
    if( total_size > 16 ) return fatal_error("Total size can only go up to 16 MB.\n");

    int ret;
    int line = 1;
    unsigned int addr;
    char rw;
    FILE * fp;

    if( (fp = fopen(argv[2],"r")) == NULL ) return fatal_error("Could not read file %s.\n",argv[2]);
    while ( (ret = fscanf(fp, "%x %c ", &addr, &rw)) == 2 )
    {
      if( rw >= 'a' ) rw -= ('a' - 'A');
      if( !(rw == 'R' || rw == 'W') )
        return fatal_error("Bad argument at line %d. Expected R or W, received %c.\n",line,rw);
      line++;
      printf("Address: %x RW: %c\n",addr,rw);
    }
    if( ret != EOF ) return fatal_error("Bad file formating at line %d.\n",line);

    return 0;
  }

  /****************************/
  /* Functions implementation */
  /****************************/

  // Indicates an error through stderr output
  // err_msg_format - format, just like in printf
  // ... - arguments, just like in printf
  // > EXIT_FAILURE
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
