
  // Page test
  // Guilherme Dantas

  #include "test.h"
  #include "page.h"

  int main ( void )
  {
    page pg = 0;

    // Initialized values are all zero
    assert(page_get_time(pg)==0);
    assert(!page_get_mflag(pg));
    assert(!page_get_rflag(pg));
    dump_page(pg);

    // Time
    int time;
    for(time = 0; time < 8; time++)
    {
      page_set_time(&pg,time);
      assert(page_get_time(pg)==time);
    }
    dump_page(pg);

    int overflow_time = ~((unsigned int)0);
    page_set_time(&pg,overflow_time);
    assert(page_get_time(pg)!=overflow_time);
    dump_page(pg);

    time = 0x3FFFFFFF; // limit
    page_set_time(&pg,time);
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Modified flag
    page_set_mflag(&pg,1);
    assert(page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Referenced flag
    page_set_rflag(&pg,1);
    assert(page_get_rflag(pg));
    assert(page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Setting modified flags back to 0
    page_set_mflag(&pg,0);
    assert(!page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Setting referenced flags back to 0
    page_set_rflag(&pg,0);
    assert(!page_get_rflag(pg));
    assert(!page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Try to overflow referenced flag
    page_set_rflag(&pg,2);
    assert(page_get_rflag(pg));
    assert(!page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // Try to overflow modified flag
    page_set_mflag(&pg,2);
    assert(page_get_rflag(pg));
    assert(page_get_mflag(pg));
    assert(page_get_time(pg)==time);
    dump_page(pg);

    // show log
    show_log();

    return 0;
  }
