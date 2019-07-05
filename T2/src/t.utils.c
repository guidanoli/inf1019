
  // Utils test
  // Guilherme Dantas

  #include "test.h"
  #include "utils.h"

  int main ( void )
  {
    // Fatal error return value
    assert(fatal_error("")!=0);

    // Power of two
    assert(!is_power_of_two(0));
    assert(is_power_of_two(1));
    assert(is_power_of_two(2));
    for(int i = 4; i <= 0xFFFFFF; i <<= 1) {
      assert(!is_power_of_two(i-1));
      assert(is_power_of_two(i));
      assert(!is_power_of_two(i+1));
    }

    // Print binary
    printf("Manual inspection:\n");
    for(int i = 0; i <= 0xF; i++) {
      print_bin(i);
    }

    show_log();

    return 0;
  }
