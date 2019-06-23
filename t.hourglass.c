
  // Hourglass module test
  // Guilherme Dantas

  #include <unistd.h>
  #include "hourglass.h"
  #include "test.h"
  #include "utils.h"

  void work() {
    for(int i = 0; i < 0xFFFFFFFF; i++); // Hard task
  }

  void see_clock() {
    printc("CLOCK",CYAN,"It's %02d:%02d:%02d.\n",
    hourglass_hours(),hourglass_minutes(),hourglass_seconds());
  }

  int main ( void )
  {
    double s;

    // Initial state
    assert(hourglass_seconds()==0);
    assert(hourglass_minutes()==0);
    assert(hourglass_hours()==0);
    assert(hourglass_total()==0.0);

    // It was not even started yet
    hourglass_stop();
    assert(hourglass_seconds()==0);
    assert(hourglass_minutes()==0);
    assert(hourglass_hours()==0);
    assert(hourglass_total()==0.0);
    see_clock();

    // Begin hourglass
    hourglass_begin();
    assert(hourglass_seconds()==0);
    assert(hourglass_minutes()==0);
    assert(hourglass_hours()==0);
    assert(hourglass_total()==0.0);
    see_clock();

    work();

    // Stop hourglass
    hourglass_stop();
    assert((s=hourglass_total())>0.0);
    see_clock();

    work();

    // Stop hourglass
    hourglass_stop();
    assert(s==hourglass_total());
    see_clock();

    // Do double work
    hourglass_begin();

    work();
    work();

    hourglass_stop();
    assert(s<hourglass_total());
    see_clock();

    show_log();
    return 0;
  }
