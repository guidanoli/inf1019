
  // Page module implementation
  // Guilherme Dantas

  #include "page.h"

  // page getters
  inline char page_get_rflag (page pg) { return (pg & 1) != 0; }
  inline char page_get_mflag (page pg) { return (pg & 2) != 0; }
  inline char page_get_pflag (page pg) { return (pg & 4) != 0; }
  inline unsigned int page_get_framenum (page pg) { return pg >> 3; }

  // page setters
  inline void page_set_rflag (page * pg, char r) {
    if ( r ) *pg |= 1; // set r to 1
    else *pg &= ~1; // set r to 0
  }

  inline void page_set_mflag (page * pg, char m) {
    if ( m ) *pg |= 2; // set m to 1
    else *pg &= ~2; // set m to 0
  }

  inline void page_set_pflag (page * pg, char p) {
    if( p ) *pg |= 4; // set p to 1
    else *pg &= ~4; // set p to 0
  }

  inline void page_set_framenum (page * pg, unsigned int framenum) {
    *pg &= 0b111; // clears framenum but preserves flags
    *pg |= framenum << 3; // updates framenum
  }

  // debug
  void dump_page (page pg) {
    char m = page_get_mflag(pg);
    char r = page_get_rflag(pg);
    char p = page_get_pflag(pg);
    unsigned int framenum = page_get_framenum(pg);
    printf("Frame number: %u Present: %s Modified: %s Referenced: %s\n",
    framenum,p?"YES":"NO",m?"YES":"NO",r?"YES":"NO");
  }
