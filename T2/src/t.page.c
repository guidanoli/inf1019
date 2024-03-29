
  // Page test
  // Guilherme Dantas

  #include <stdlib.h>
  #include "test.h"
  #include "page.h"

  int main ( void )
  {
    page_t * pg = (page_t *) malloc(sizeof(page_t));
    fatal_assert(pg); 

    // Initialized values are all zero
    assert(!page_get_mflag(*pg));
    assert(!page_get_rflag(*pg));
    assert(!page_get_pflag(*pg));
    dump_page(*pg);

    // Modified flag
    page_set_mflag(pg,1);
    assert(!page_get_pflag(*pg));
    assert(!page_get_rflag(*pg));
    assert(page_get_mflag(*pg));
    dump_page(*pg);

    // Referenced flag
    page_set_rflag(pg,1);
    assert(!page_get_pflag(*pg));
    assert(page_get_rflag(*pg));
    assert(page_get_mflag(*pg));
    dump_page(*pg);

    // Present flag
    page_set_pflag(pg,1);
    assert(page_get_pflag(*pg));
    assert(page_get_rflag(*pg));
    assert(page_get_mflag(*pg));
    dump_page(*pg);

    // Setting modified flag back to 0
    page_set_mflag(pg,0);
    assert(page_get_pflag(*pg));
    assert(page_get_rflag(*pg));
    assert(!page_get_mflag(*pg));
    dump_page(*pg);

    // Setting referenced flag back to 0
    page_set_rflag(pg,0);
    assert(page_get_pflag(*pg));
    assert(!page_get_rflag(*pg));
    assert(!page_get_mflag(*pg));
    dump_page(*pg);

    // Setting present flag back to 0
    page_set_pflag(pg,0);
    assert(!page_get_pflag(*pg));
    assert(!page_get_rflag(*pg));
    assert(!page_get_mflag(*pg));
    dump_page(*pg);

    // Try to overflow present flag
    page_set_pflag(pg,2);
    assert(page_get_pflag(*pg));
    assert(!page_get_rflag(*pg));
    assert(!page_get_mflag(*pg));
    dump_page(*pg);

    // Try to overflow referenced flag
    page_set_rflag(pg,2);
    assert(page_get_pflag(*pg));
    assert(page_get_rflag(*pg));
    assert(!page_get_mflag(*pg));
    dump_page(*pg);

    // Try to overflow modified flag
    page_set_mflag(pg,2);
    assert(page_get_pflag(*pg));
    assert(page_get_rflag(*pg));
    assert(page_get_mflag(*pg));
    dump_page(*pg);

    // show log
    show_log();

    return 0;
  }
