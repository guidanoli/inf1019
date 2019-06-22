
  // Main Module
  // Guilherme Dantas

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include "page.h"
  #include "utils.h"

  #define ARGS "<program> <algorithm> <log path> <page size> <total size> [-D]"
  #define ALG_INIT() alg_init[(int) algorithm]()
  #define ALG_ACCESS(pgid,rw) alg_access[(int) algorithm]((unsigned int)pgid,(rw_t)rw)
  #define ALG_DESTROY(p) alg_destroy[(int) algorithm](p)

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
  static void page_table_init (unsigned int s);
  static void page_table_destroy ();
  static int safe_fatal_error ();

  static void nru_init ();
  static void lru_init ();
  static void novo_init ();

  static void nru_destroy (page_t * page);
  static void lru_destroy (page_t * page);
  static void novo_destroy (page_t * page);

  static access_t nru_access (unsigned int page_index, rw_t rw);
  static access_t lru_access (unsigned int page_index, rw_t rw);
  static access_t novo_access (unsigned int page_index, rw_t rw);

  /********************/
  /* Global variables */
  /********************/

  char debug;
  FILE * fp;

  algorithm_t algorithm = -1;
  access_t (* alg_access[3])(unsigned int,rw_t) = {nru_access,lru_access,novo_access};
  void (* alg_init[3])() = {nru_init,lru_init,novo_init};
  void (* alg_destroy[3])(void *) = {nru_destroy,lru_destroy,novo_destroy};
  const char * alg_name[3] = {"NRU","LRU","NOVO"};

  unsigned int time = 0;        // time counter
  unsigned int faults_cnt = 0;  // #pages that caused page fault
  unsigned int dirty_cnt = 0;   // #pages written back to memory

  unsigned int page_cnt;        // #pages in page table
  unsigned int max_page_cnt;    // #pages in memory
  unsigned int page_size;       // page size in KB
  unsigned int total_size;      // memory size in MB

  page_t * page_table;

  /********/
  /* Main */
  /********/

  int main (int argc, char ** argv)
  {
    // Arguments validation
    if( argc != 5 && argc != 6 ) return fatal_error("Invalid parameters.\nExpected: %s\n", ARGS);
    for( int i = 0 ; i < 3; i++ ) if( !strcmp(argv[1],alg_name[i]) ) algorithm = (algorithm_t) i;
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

    page_table_init(s);
    // File processing and algorithm calling
    if( (fp = fopen(argv[2],"r")) == NULL ) return fatal_error("Could not read file %s.\n",argv[2]);
    while ( (ret = fscanf(fp, "%x %c ", &addr, &rw)) == 2 )
    {
      unsigned int page_index = addr >> s;
      if( rw >= 'a' && rw <= 'z' ) rw -= ('a' - 'A'); // to upper case
      if( !(rw == READ || rw == WRITE) ) return safe_fatal_error();
      access_t access = ALG_ACCESS(page_index,rw);
      if( access & PAGE_FAULT ) faults_cnt++;
      if( access & PAGE_DIRTY ) dirty_cnt++;
      time++;
    }
    fclose(fp);
    page_table_destroy();
    if( ret != EOF ) return fatal_error("Bad file formating at line %u.\n",time+1);

    return 0;
  }

  /****************************/
  /* Functions implementation */
  /****************************/

    /***************************/
    /* NRU - Not Recently Used */
    /***************************/

  static void nru_init ()
  {

  }

  static void nru_destroy (page_t * page)
  {

  }

  static access_t nru_access (unsigned int page_index, rw_t rw)
  {
    printf("%u %c\n",page_index,rw);
    return PAGE_HIT; // TEMP
  }

    /*****************************/
    /* LRU - Least Recently Used */
    /*****************************/

  static void lru_init ()
  {

  }

  static void lru_destroy (page_t * page)
  {

  }

  static access_t lru_access (unsigned int page_index, rw_t rw)
  {

    return PAGE_HIT; // TEMP
  }

    /******************/
    /* NOVO - Optimal */
    /******************/

  static void novo_init ()
  {

  }

  static void novo_destroy (page_t * page)
  {

  }

  static access_t novo_access (unsigned int page_index, rw_t rw)
  {

    return PAGE_HIT; // TEMP
  }

    /***************************************/
    /* General functions for any algorithm */
    /***************************************/

  static int safe_fatal_error ()
  {
    fclose(fp);
    page_table_destroy();
    return fatal_error("Bad argument at line %u.\n",time+1);
  }

  static void page_table_init (unsigned int s)
  {
    page_cnt = 1 << (32 - s);
    page_table = (page_t *) malloc( sizeof(page_t) * page_cnt );
    if( page_table == NULL )
    {
      fatal_error("Could not allocate memory to page table.\n");
      exit(1);
    }
    for( int i = 0; i < page_cnt; i++ ) page_table[i].flags = 0;
    ALG_INIT();
  }

  static void page_table_destroy ()
  {
    for( int i = 0; i < page_cnt; i++ ) ALG_DESTROY(&page_table[i]);
    free(page_table);
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
