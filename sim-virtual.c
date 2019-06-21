
  // Main Module
  // Guilherme Dantas

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include "page.h"
  #include "utils.h"

  #define PARAMS "<program> <algorithm> <log path> <page size> <total size> [-D]"

  typedef enum {
    NRU,
    LRU,
    NOVO,
  } algorithm_t;

  typedef enum {
    READ = (int) 'R',
    WRITE = (int) 'W',
  } rw_t;

  typedef enum {
    PAGE_HIT = 0b0,
    PAGE_FAULT = 0b1,
    PAGE_DIRTY = 0b10,
  } access_t;

  /********************/
  /* Static functions */
  /********************/

  static int get_s (int page_size);
  static access_t nru (unsigned int page_index, rw_t rw);
  static access_t lru (unsigned int page_index, rw_t rw);
  static access_t novo (unsigned int page_index, rw_t rw);

  /********************/
  /* Global variables */
  /********************/

  char debug;

  algorithm_t algorithm = -1;
  access_t (*algorithm_functions[3])(unsigned int,rw_t) = {nru,lru,novo};
  const char * algorithm_names[3] = {"NRU","LRU","NOVO"};

  unsigned int time = 0;
  unsigned int faults_cnt = 0;
  unsigned int dirty_cnt = 0;

  int page_size; // in KB
  int total_size; // in MB

  /********/
  /* Main */
  /********/

  int main (int argc, char ** argv)
  {
    // Arguments validation
    if( argc != 5 && argc != 6 ) return fatal_error("Invalid parameters.\nExpected: %s\n", PARAMS);
    for(int i = 0 ; i < 3; i++) if( !strcmp(argv[1],algorithm_names[i]) ) algorithm = (algorithm_t) i;
    if( algorithm == -1 ) return fatal_error("Invalid algorithm.\nValid values: LRU, NRU, NOVO\n");
    if( access(argv[2],F_OK) == -1 ) return fatal_error("File %s does not exist.\n",argv[2]);
    if( (page_size = atoi(argv[3])) <= 0 ) return fatal_error("Invalid page size\n");
    if( page_size < 8 || page_size > 32 ) return fatal_error("Page size must be between 8 and 32 KB.\n");
    if( !is_power_of_two(page_size) ) return fatal_error("Page size must be a power of 2.\n");
    if( (total_size = atoi(argv[4])) <= 0 ) return fatal_error("Invalid total size\n");
    if( total_size > 16 ) return fatal_error("Total size can only go up to 16 MB.\n");
    if( !is_power_of_two(total_size) ) return fatal_error("Total size must be a power of 2.\n");
    if( argc == 6 ) debug = strcmp(argv[5],"-D") == 0;

    if( debug ) printf("[DEBUG MODE]\n");

    int ret;
    unsigned int addr;
    unsigned int s = get_s(page_size);
    char rw;
    FILE * fp;

    // File processing and algorithm calling
    if( (fp = fopen(argv[2],"r")) == NULL ) return fatal_error("Could not read file %s.\n",argv[2]);
    while ( (ret = fscanf(fp, "%x %c ", &addr, &rw)) == 2 )
    {
      unsigned int page_index = addr >> s;
      if( rw >= 'a' && rw <= 'z' ) rw -= ('a' - 'A'); // to upper case
      if( !(rw == READ || rw == WRITE) ) return fatal_error("Bad argument at line %d.\n",time+1);
      access_t access = algorithm_functions[(int) algorithm](page_index,(rw_t)rw);
      if( access & PAGE_FAULT ) faults_cnt++;
      if( access & PAGE_DIRTY ) dirty_cnt++;
      time++;
    }
    fclose(fp);
    if( ret != EOF ) return fatal_error("Bad file formating at line %d.\n",time+1);

    return 0;
  }

  /****************************/
  /* Functions implementation */
  /****************************/

  static access_t nru (unsigned int page_index, rw_t rw)
  {
    printf("%u %c\n",page_index,rw);
    return PAGE_HIT; // TEMP
  }

  static access_t lru (unsigned int page_index, rw_t rw)
  {

    return PAGE_HIT; // TEMP
  }

  static access_t novo (unsigned int page_index, rw_t rw)
  {

    return PAGE_HIT; // TEMP
  }

  // Calculate s (shift done in physical address
  // to obtain logical address)
  // page_size - page size, in KB
  // > s
  // [!] It is assumed that the page is a power of 2
  static int get_s (int page_size)
  {
    int s = 10; // in KB
    while( page_size > 1 )
    {
      page_size >>= 1;
      s++;
    }
    return s;
  }
