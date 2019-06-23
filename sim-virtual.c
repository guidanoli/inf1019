
  // Main Module
  // Guilherme Dantas

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <signal.h>
  #include <time.h>
  #include "page.h"
  #include "list.h"
  #include "utils.h"
  #include "hourglass.h"

  #define ARGS "<program> <algorithm> <log path> <page size> <total size> [-D]"
  #define ALG_INIT()          alg_init[algorithm]     ()
  #define ALG_FAULT(p)        alg_fault[algorithm]    (p)
  #define ALG_UPDATE(p)       alg_update[algorithm]   (p)
  #define ALG_DESTROY()       alg_destroy[algorithm]  ()

  #define NRU_CYCLES 0x10000000

  typedef enum {
    NRU,
    LRU,
    NOVO,
  } algorithm_t;

  typedef enum {
    READ = (int) 'R',
    WRITE = (int) 'W',
  } rw_t;

  /********************/
  /* Static functions */
  /********************/

  static int get_s (int page_size);
  static unsigned int get_present_page_count ();
  static void page_table_init (unsigned int s);
  static void page_table_destroy ();
  static int safe_fatal_error (const char * errmsg);
  static void show_statistics ();
  void sig_handler(int signo);

  static void nru_init ();
  static void lru_init ();
  static void novo_init ();

  static void nru_update (page_t * page);
  static void lru_update (page_t * page);
  static void novo_update (page_t * page);

  static page_t * nru_fault (page_t * page);
  static page_t * lru_fault (page_t * page);
  static page_t * novo_fault (page_t * page);

  static void nru_destroy ();
  static void lru_destroy ();
  static void novo_destroy ();

  /********************/
  /* Global variables */
  /********************/

  char debug;
  FILE * fp;

  algorithm_t algorithm = -1;
  void (* alg_init[3])() = {nru_init,lru_init,novo_init};
  page_t * (* alg_fault[3])(page_t * page) = {nru_fault,lru_fault,novo_fault};
  void (* alg_update[3])(page_t * page) = {nru_update,lru_update,novo_update};
  void (* alg_destroy[3])() = {nru_destroy,lru_destroy,novo_destroy};
  const char * alg_name[3] = {"NRU","LRU","NOVO"};

  int * nru_victims = NULL;
  int nru_victims_cnt = 0;

  unsigned int tcounter = 0;        // time counter
  unsigned int faults_cnt = 0;  // #pages that caused page fault
  unsigned int dirty_cnt = 0;   // #pages written back to memory

  unsigned int table_size;      // #pages in page table
  unsigned int page_size;       // page size in KB
  unsigned int total_size;      // memory size in MB
  unsigned int max_page_cnt;    // maximum number of pages

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

    if (signal(SIGINT, sig_handler) == SIG_ERR) return fatal_error("Can't catch SIGINT.\n");
    srand(time(NULL));

    max_page_cnt = (total_size << 10) / page_size;
    if( debug ) printc("Simulator",CYAN,"Executing on debug mode...\n");
    else printc("Simulator",CYAN,"Executing...\n");
    printc("Simulator",CYAN,"Input file: %s\n",argv[2]);
    printc("Simulator",CYAN,"Physic Memory size: %u\n",total_size);
    printc("Simulator",CYAN,"Page size: %u\n",page_size);
    printc("Simulator",CYAN,"Page Replacement Algorithm: %s\n",argv[1]);

    int ret;
    unsigned int addr;
    unsigned int s = get_s(page_size);
    char rw;

    int debug_iterations = 0;

    hourglass_begin();
    page_table_init(s);
    // File processing and algorithm calling
    if( (fp = fopen(argv[2],"r")) == NULL ) return fatal_error("Could not read file %s.\n",argv[2]);
    while ( (ret = fscanf(fp, "%x %c ", &addr, &rw)) == 2 )
    {
      // Debug iterative mode checker
      if( debug )
      {
        if( debug_iterations == 0 )
        {
          printc("Debug",GREEN,">>> Number of iterations: ");
          if( scanf("%d", &debug_iterations) != 1 || debug_iterations <= 0)
            debug_iterations = 1;
        }
        debug_iterations--;
      }

      // Page index calculation and r/w validation
      unsigned int page_index = addr >> s;
      if( rw >= 'a' && rw <= 'z' ) rw -= ('a' - 'A'); // to upper case
      if( !(rw == READ || rw == WRITE) ) return safe_fatal_error("Bad r/w argument");

      if( debug ) printc("Page",YELLOW,"Access to %u for %s\n",page_index,rw==READ?"R":"R/W");

      // Page access
      page_t * page = &page_table[page_index];
      if( page_get_pflag(*page) )
      {
        if( debug ) printc("Page",YELLOW,"Page hit!\n");
      }
      else
      {
        if( debug ) printc("Page",YELLOW,"Page fault!\n");
        if( get_present_page_count() < max_page_cnt )
        {
          if( debug ) printc("Page",YELLOW,"There is space for the page on memory!\n");
        }
        else
        {
          if( debug ) printc("Page",YELLOW,"There isn't space for the page on memory, so a victim will be evited!\n");

          page_t * victim = ALG_FAULT(page);

          // Victim page
          page_set_pflag(victim,0);
          page_set_rflag(victim,0);
          if( page_get_mflag(*victim) ){
            dirty_cnt++;
            if( debug ) printc("Page",YELLOW,"The victim page was dirty and has been written in disk now!\n");
          }
          page_set_mflag(victim,0);
        }

        // Current page
        page_set_pflag(page,1);
        page_set_rflag(page,1);

        faults_cnt++;
      }
      if( rw == WRITE )
      {
        page_set_mflag(page,1);
        if( debug ) printc("Page",YELLOW,"The page has been modified!\n");
      }
      ALG_UPDATE(page);
      tcounter++;
    }
    fclose(fp);
    page_table_destroy();
    show_statistics();
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
    nru_victims = (int *) malloc ( sizeof(int) * max_page_cnt );
  }

  static page_t * nru_fault (page_t * page)
  {
    int best_victim = 0;
    int best_class = 0;
    nru_victims_cnt = 0;

    for( int i = 0; i < table_size ; i++ )
    {
      page_t victim = page_table[i];
      if( !page_get_pflag(victim) ) continue;
      int class = (page_get_rflag(victim) << 1) | page_get_mflag(victim);
      if( class > best_class )
      {
        nru_victims_cnt = 0;
        best_class = class;
      }
      if( class == best_class )
      {
        nru_victims[nru_victims_cnt] = i;
        nru_victims_cnt++;
      }
    }

    if( debug ) printc("NRU",MAGENTA,"There are %u possible pages to be evicted.\n",nru_victims_cnt);

    int index = rand() % nru_victims_cnt;
    return page_table + nru_victims[index];
  }

  static void nru_update (page_t * page)
  {
    if( tcounter % NRU_CYCLES == 0 )
    {
      page_t * temp = page_table;
      for( int i = 0 ; i < table_size; i++ )
      {
        page_set_rflag(temp,0);
        temp++;
      }
    }
    page_set_rflag(page,1);
  }

  static void nru_destroy ()
  {
    free(nru_victims);
  }

    /*****************************/
    /* LRU - Least Recently Used */
    /*****************************/

  static void lru_init ()
  {
    // nothing
  }

  static page_t * lru_fault (page_t * page)
  {
    // nothing
    return NULL;
  }

  static void lru_update (page_t * page)
  {
    // nothing
  }

  static void lru_destroy ()
  {
    // nothing
  }

    /******************/
    /* NOVO - Optimal */
    /******************/

  static void novo_init ()
  {
    // nothing
  }

  static page_t * novo_fault (page_t * page)
  {
    // nothing
    return NULL;
  }

  static void novo_update (page_t * page)
  {
    // nothing
  }

  static void novo_destroy ()
  {
    // nothing
  }

    /***************************************/
    /* General functions for any algorithm */
    /***************************************/

  static int safe_fatal_error (const char * errmsg)
  {
    fclose(fp);
    page_table_destroy();
    show_statistics();
    return fatal_error("Error at line %u: %s\n",time+1,errmsg);
  }

  static void page_table_init (unsigned int s)
  {
    table_size = 1 << (32 - s);
    page_table = (page_t *) malloc( sizeof(page_t) * table_size );
    if( page_table == NULL )
    {
      fatal_error("Could not allocate memory to page table.\n");
      exit(1);
    }
    for( int i = 0; i < table_size; i++ ) page_table[i].flags = 0;
    ALG_INIT();
  }

  static void page_table_destroy ()
  {
    ALG_DESTROY();
    free(page_table);
  }

  static unsigned int get_present_page_count ()
  {
    unsigned int count = 0;
    for( int i = 0 ; i < table_size; i++ )
      count += page_get_pflag(page_table[i]);
    return count;
  }

  static void show_statistics ()
  {
    hourglass_stop();
    double s = hourglass_total();
    int m = hourglass_minutes();
    int h = hourglass_hours();
    float fperc = (float)faults_cnt*100/(float)tcounter;
    float dperc = (float)dirty_cnt*100/(float)tcounter;
    double tperaccess = s/(double)tcounter;
    char magnitude;
    if( tperaccess >= 1 ) { magnitude = ' '; }
    else if( tperaccess >= 1e-3 ) { magnitude = 'm'; tperaccess *= 1e3; }
    else if( tperaccess >= 1e-6 ) { magnitude = 'u'; tperaccess *= 1e6; }
    else if( tperaccess >= 1e-9 ) { magnitude = 'n'; tperaccess *= 1e9; }
    else { magnitude = 'p'; tperaccess *= 1e12; } // Almost impossible

    printc("Simulator",CYAN,"Number of page faults: %u (%.2f%%)\n",faults_cnt,fperc);
    printc("Simulator",CYAN,"Number of pages written: %u (%.2f%%)\n",dirty_cnt,dperc);
    printc("Simulator",CYAN,"Done in %02d:%02d:%02.4lf (%.1lf%cs/access).\n",h,m,s,tperaccess,magnitude);
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

  void sig_handler(int signo)
  {
    printf("\n"); // jumps a line
    safe_fatal_error("The program has been interrupted safely.");
    exit(1);
  }
