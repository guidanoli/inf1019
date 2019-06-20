
  // Page module implementation
  // Guilherme Dantas

  #include "page.h"

  // page getters
  inline char get_rflag (page pg) { return pg & 1; }
  inline char get_mflag (page pg) { return pg & 2; }
  inline unsigned int get_time (page pg) { return pg >> 2; }

  // page setters
  inline void set_rflag (page * pg, char r) {
    if ( r ) *pg |= 1; // set r to 1
    else *pg &= ~1; // set r to 0
  }

  inline void set_mflag (page * pg, char m) {
    if ( m ) *pg |= 2; // set m to 1
    else *pg &= ~2; // set m to 0
  }

  inline void set_time (page * pg, unsigned int time) {
    *pg &= ~3; // clears time but preserves flags
    *pg |= time << 2; // updates time
  }
