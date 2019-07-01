
  // Page module interface
  // Guilherme Dantas

#ifndef PAGE_H
#define PAGE_H

  // A page has the following fields:
  // Referenced - aka 'R' flag
  // Modified - aka 'M' flag
  // Present - aka 'P' flag
  typedef unsigned char page_flags_t;

  //    The page structure holds not only the
  // flags but also additional information that
  // can be useful for the replacement algorithm
  //    Of course, void * can be manouvered and used
  // simply for a 64-bit field like a counter (LRU)
  // It is safe to do the later because the info
  // pointer is never accessed by the page module
  // internally, so no segmentation fault will occur.
  struct page_s {
    page_flags_t flags;
    void * info;
  };

  typedef struct page_s page_t;

  // Fields getters
  extern char page_get_rflag (page_t pg);
  extern char page_get_mflag (page_t pg);
  extern char page_get_pflag (page_t pg);

  // Fields setters
  extern void page_set_rflag (page_t * pg, char r);
  extern void page_set_mflag (page_t * pg, char m);
  extern void page_set_pflag (page_t * pg, char p);

  // Debug
  void dump_page (page_t pg);

#endif
