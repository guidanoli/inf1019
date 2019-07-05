
  // Hourglass module interface
  // Guilherme Dantas

  #ifndef HOURGLASS_H
  #define HOURGLASS_H

  // Begin counting time
  void hourglass_begin ();

  // Stop counting time
  void hourglass_stop ();

  // Get seconds (< 60)
  unsigned int hourglass_seconds ();

  // Get minutes (< 60)
  unsigned int hourglass_minutes ();

  // Get hours
  unsigned int hourglass_hours ();

  // Get total time elapsed (in seconds)
  double hourglass_total ();

  #endif
