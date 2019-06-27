# Virtual Memory Simulator

The goal of this project is to implement a virtual memory simulator. Memory access will be simulted through files in which
lines are composed of the physical adress of the access and a char to determine if the access is Read-Only or Read/Write.
The simulator accepts the following page algorithms:

* Not Recently Used (NRU)
* Least Recently Used (LRU)
* Optimal Algorithm (NOVO, stands for *NEW* in Portuguese)

## Make

In order to generate the executable files, simply do:

``` bash
$ make
```

Or, to generate the tests:

``` bash
$ make tests
```

And to clean the directory to its initial state:

``` bash
$ make clear
```

## Executing

The simulator takes the following parameters:

``` bash
$ ./sim-virtual <algorithm> <log path> <page size> <total size> [-D]
```

* **algorithm** - one of the following: NRU, LRU, NOVO (case-sensitive)
* **log path** - path to log file (with format as descripted above)
* **page size** - size of page in KB (8K, 16K, 32K)
* **total size** - total size in memory in MB (1MB, 2MB, 4MB, 8MB, 16MB)
* **D flag** - run simulator on debug mode (iterative and verbose)

You can safely interrupt the program mid its execution with Ctrl+C.
At the end of the execution, the program will prompt some statistics.
