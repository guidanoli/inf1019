# Multi-level Scheduling with Feedback

The goal of this project is to implement a command interpreter and process scheduler in C programming language, making use of not only the standard C library (mainly signal.h, pthread.h and sys/sem.h) but also modules implemented solely by me. The list that follows might guide you, newcomer, through this project folder.

## Files

* **algorithm.txt**     Studying the algorithm behind the scheduler
* **main.c**            Interpreter and scheduler source code
* **Makefile**          Self explanatory
* **prog.c**            User program source code
* **queue.c**           Queue module source code
* **queue.h**           Queue module header
* **semlib.c**          Semaphore module source code
* **semlib.h**          Semaphore module header
* **test_queue.c**      Queue module automatic test source code

## Make

In order to generate the binaries, you should choose between one of the following available distributions.

### Release (less verbose)

```bash
$ make -B
```

### Debug (more verbose)

```bash
$ make debug -B
```

**PS: The -B flag is just to overwrite any files when switching from distributions**

## Credits

The INF1019 Computing Systems course is oriented by the professor Luiz Fernando Bessa Seibel The proper credits for the semaphore module can be found either in its source code or in its header.
