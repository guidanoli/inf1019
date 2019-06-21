
  // Page module interface
  // Guilherme Dantas

#ifndef PAGE_H
#define PAGE_H

  // A page has the following fields:
  // Frame number
  // Referenced - aka 'R' flag
  // Modified - aka 'M' flag
  // Present - aka 'P' flag
  typedef unsigned int page;

  // Fields getters
  extern char page_get_rflag (page pg);
  extern char page_get_mflag (page pg);
  extern char page_get_pflag (page pg);
  extern unsigned int page_get_framenum (page pg);

  // Fields setters
  extern void page_set_rflag (page * pg, char r);
  extern void page_set_mflag (page * pg, char m);
  extern void page_set_pflag (page * pg, char p);
  extern void page_set_framenum (page * pg, unsigned int framenum);

  // Debug
  void dump_page (page pg);

#endif
