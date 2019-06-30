# Virtual Memory Simulator

The goal of this project is to examine the pros and cons of each **page replacement algorithm** for various page sizes, physical memory size and type of program accessing memory - comparing it to the optimal algorithm - in a safe simulating environment, with controlled data structures, avoiding memory leak and file locks. The simulator reads memory accesses as lines composed of the physical address and a character to determine whether the access is Read-Only or Read/Write. It currently accepts the following page algorithms, whose implementations are based on the well-known *Modern Operating Systems* book by **Andrew S. Tanenbaum & Hebert Bos**.

* *NRU* - Not Recently Used
* *LRU* - Least Recently Used
* *OPT* - Optimal Algorithm

## Getting Started

The following instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

This project was first implemented on a machine running the **Fedora Thirty**, and I haven't tested it in a Windows environment (but there are have several reasons it wouldn't even compile on it). So, unless you'd like to fork and make it Windows compatible, you can only run this program on machines running on a **POSIX** operating system. :)

### Installing

Make use of the Makefile in your favour. These are the currently accepted targets:

``` bash
$ make [sim|tests|clear|clean]
```

* **sim** - generates simulator
* **tests** - generate tests
* **clear|clean** - removes files generated by compilation

## Running the tests

The tests modules test the modules according to its interfaces, thus - a black box testing paradigm. E.g:

``` c
// Power of two
assert(!is_power_of_two(0));
assert(is_power_of_two(1));
assert(is_power_of_two(2));
```

The tests all inherit the automatic test module interface, that helps with an enhanced and colourful log displaying not only the assertions but the code that generated the error and the line its located at on the test module. E.g:

``` bash
[SUCCESS] !is_power_of_two(0)
[SUCCESS] is_power_of_two(1)
[SUCCESS] is_power_of_two(2)
```

**Disclaimer:** All test source code and executables begin with *t.* and this prefix should be exclusive to these files!

## Executing

The simulator takes the following parameters:

``` bash
$ ./sim-virtual <algorithm> <log path> <page size> <total size> [-D]
```

* **algorithm** - one of the currently page replacement algorithms identifiers.
* **log path** - path to log file (with format as described above)
* **page size** - size of page in KB (8K, 16K, 32K)
* **total size** - total size in memory in MB (1MB, 2MB, 4MB, 8MB, 16MB)
* **D flag** - run simulator on debug mode (iterative and verbose)

You can safely interrupt the program mid its execution with Ctrl+C. At the end of its execution, the program will prompt some statistics.

## Log files

Since the log files are quite large and I avoid adding large files to the repository folder, I decided to host it elsewhere. You can download the .zip file with the four log files on the following link section. Be aware that if you want to run the script (getresults.sh), you must first give it executing permission (chmod +x getresults.sh), have a Python interpreter installed and have all the four log files on the folder root.

## Links

[Seibel (*course supervisor*)](http://139.82.24.35/seibel/hp/)
[Statement (*in Portuguese*)](https://drive.google.com/file/d/1fUyadkXQwtmSAcDa2FTxC0TYexYvmR2e/view?usp=sharing)
[Log files (*for testing*)](https://drive.google.com/file/d/1s7By6hcJJ8N41QByLZVRp0rr6gXxdhQl/view?usp=sharing)
