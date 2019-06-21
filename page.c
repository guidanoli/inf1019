
  // Page module implementation
  // Guilherme Dantas

  #include "page.h"

  // page getters
  inline char page_get_rflag (page pg) { return (pg & 1) != 0; }
  inline char page_get_mflag (page pg) { return (pg & 2) != 0; }
  inline unsigned int page_get_time (page pg) { return pg >> 2; }

  // page setters
  inline void page_set_rflag (page * pg, char r) {
    if ( r ) *pg |= 1; // set r to 1
    else *pg &= ~1; // set r to 0
  }

  inline void page_set_mflag (page * pg, char m) {
    if ( m ) *pg |= 2; // set m to 1
    else *pg &= ~2; // set m to 0
  }

  inline void page_set_time (page * pg, unsigned int time) {
    *pg &= 3; // clears time but preserves flags
    *pg |= time << 2; // updates time
  }

  // debug
  void dump_page (page pg) {
    char m = page_get_mflag(pg);
    char r = page_get_rflag(pg);
    unsigned int time = page_get_time(pg);
    printf("Time: %u Modified: %s Referenced: %s\n",
    time,m?"YES":"NO",r?"YES":"NO");
  }
