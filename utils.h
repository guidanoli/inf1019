
  // Utilities module interface
  // Guilherme Dantas

#ifndef UTILS_H
#define UTILS_H

  // Indicates an error through stderr output
  // err_msg_format - format, just like in printf
  // ... - arguments, just like in printf
  // > EXIT_FAILURE
  int fatal_error (const char * err_msg_format, ...);

  // Check if n = 2^m, m being a natural number
  // n - number
  // > 0, 1
  int is_power_of_two (int n);

  // Print number in binary notation plus new line
  // n - number
  // > (in stdout) binary representation of n
  void print_bin (int n);

#endif
