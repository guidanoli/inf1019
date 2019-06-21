
  // Page test
  // Guilherme Dantas

  #include "test.h"
  #include "page.h"

  int main ( void )
  {
    page_t pg = 0;

    // Initialized values are all zero
    assert(page_get_framenum(pg)==0);
    assert(!page_get_mflag(pg));
    assert(!page_get_rflag(pg));
    assert(!page_get_pflag(pg));
    dump_page(pg);

    // Time
    int framenum;
    for(framenum = 0; framenum < 8; framenum++)
    {
      page_set_framenum(&pg,framenum);
      assert(page_get_framenum(pg)==framenum);
    }
    dump_page(pg);

    int overflow_framenum = ~((unsigned int)0);
    page_set_framenum(&pg,overflow_framenum);
    assert(page_get_framenum(pg)!=overflow_framenum);
    dump_page(pg);

    framenum = 0x1FFFFFFF; // limit
    page_set_framenum(&pg,framenum);
    assert(page_get_framenum(pg)==framenum);
    dump_page(pg);

    // Modified flag
    page_set_mflag(&pg,1);
    assert(!page_get_pflag(pg));
    assert(!page_get_rflag(pg));
    assert(page_get_mflag(pg));
    assert(page_get_framenum(pg)==framenum);
    dump_page(pg);

    // Referenced flag
    page_set_rflag(&pg,1);
    assert(!page_get_pflag(pg));
    assert(page_get_rflag(pg));
    assert(page_get_mflag(pg));
    assert(page_get_framenum(pg)==framenum);
    dump_page(pg);

    // Present flag
    page_set_pflag(&pg,1);
    assert(page_get_pflag(pg));
    assert(page_get_rflag(pg));
    assert(page_get_mflag(pg));
    assert(page_get_framenum(pg)==framenum);
    dump_page(pg);

    // Setting modified flag back to 0
    page_set_mflag(&pg,0);
    assert(page_get_pflag(pg));
    assert(page_get_rflag(pg));
    assert(!page_get_mflag(pg));
    assert(page_get_framenum(pg)==framenum);
    dump_page(pg);

    // Setting referenced flag back to 0
    page_set_rflag(&pg,0);
    assert(page_get_pflag(pg));
    assert(!page_get_rflag(pg));
    assert(!page_get_mflag(pg));
    assert(page_get_framenum(pg)==framenum);
    dump_page(pg);

    // Setting present flag back to 0
    page_set_pflag(&pg,0);
    assert(!page_get_pflag(pg));
    assert(!page_get_rflag(pg));
    assert(!page_get_mflag(pg));
    assert(page_get_framenum(pg)==framenum);
    dump_page(pg);

    // Try to overflow present flag
    page_set_pflag(&pg,2);
    assert(page_get_pflag(pg));
    assert(!page_get_rflag(pg));
    assert(!page_get_mflag(pg));
    assert(page_get_framenum(pg)==framenum);
    dump_page(pg);

    // Try to overflow referenced flag
    page_set_rflag(&pg,2);
    assert(page_get_pflag(pg));
    assert(page_get_rflag(pg));
    assert(!page_get_mflag(pg));
    assert(page_get_framenum(pg)==framenum);
    dump_page(pg);

    // Try to overflow modified flag
    page_set_mflag(&pg,2);
    assert(page_get_pflag(pg));
    assert(page_get_rflag(pg));
    assert(page_get_mflag(pg));
    assert(page_get_framenum(pg)==framenum);
    dump_page(pg);

    // show log
    show_log();

    return 0;
  }
