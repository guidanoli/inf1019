
  // Page module implementation
  // Guilherme Dantas

  #include "page.h"

  // page getters
  inline char page_get_rflag (page_t pg) { return (pg.flags & 1) != 0; }
  inline char page_get_mflag (page_t pg) { return (pg.flags & 2) != 0; }
  inline char page_get_pflag (page_t pg) { return (pg.flags & 4) != 0; }

  // page setters
  inline void page_set_rflag (page_t * pg, char r) {
    if ( r ) pg->flags |= 1; // set r to 1
    else pg->flags &= ~1; // set r to 0
  }

  inline void page_set_mflag (page_t * pg, char m) {
    if ( m ) pg->flags |= 2; // set m to 1
    else pg->flags &= ~2; // set m to 0
  }

  inline void page_set_pflag (page_t * pg, char p) {
    if( p ) pg->flags |= 4; // set p to 1
    else pg->flags &= ~4; // set p to 0
  }

  // debug
  void dump_page (page_t pg) {
    char m = page_get_mflag(pg);
    char r = page_get_rflag(pg);
    char p = page_get_pflag(pg);
    printf("Present: %s Modified: %s Referenced: %s\n",
    p?"YES":"NO",m?"YES":"NO",r?"YES":"NO");
  }
