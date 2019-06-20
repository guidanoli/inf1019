
  // Page module interface
  // Guilherme Dantas

  typedef unsigned int page;

  inline char get_rflag (page pg);
  inline char get_mflag (page pg);
  inline unsigned int get_time (page pg);
  inline void set_rflag (page * pg, char r);
  inline void set_mflag (page * pg, char m);
  inline void set_time (page * pg, unsigned int time);
