
  // Page module interface
  // Guilherme Dantas

#ifndef PAGE_H
#define PAGE_H

  typedef unsigned int page;

  extern char page_get_rflag (page pg);
  extern char page_get_mflag (page pg);
  extern unsigned int page_get_time (page pg);
  extern void page_set_rflag (page * pg, char r);
  extern void page_set_mflag (page * pg, char m);
  extern void page_set_time (page * pg, unsigned int time);
  void dump_page (page pg);

#endif
