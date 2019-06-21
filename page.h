
  // Page module interface
  // Guilherme Dantas

#ifndef PAGE_H
#define PAGE_H

  // A page has the following fields:
  // Frame number
  // Referenced - aka 'R' flag
  // Modified - aka 'M' flag
  // Present - aka 'P' flag
  typedef unsigned int page_t;

  // Fields getters
  extern char page_get_rflag (page_t pg);
  extern char page_get_mflag (page_t pg);
  extern char page_get_pflag (page_t pg);
  extern unsigned int page_get_framenum (page_t pg);

  // Fields setters
  extern void page_set_rflag (page_t * pg, char r);
  extern void page_set_mflag (page_t * pg, char m);
  extern void page_set_pflag (page_t * pg, char p);
  extern void page_set_framenum (page_t * pg, unsigned int framenum);

  // Debug
  void dump_page (page_t pg);

#endif
