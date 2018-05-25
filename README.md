There are two programs, `upcxx-partitioned-map` and `upcxx-wordcount`. The difference between them is explained in the report.

There are 3 optional arguments  
`./kv <FILENAME> <INITIAL HASHTABLE SIZE> <MAX SEPARATE CHAINING BEFORE EXPAND>`
Default filename (when running without arguments like in the examples below) can be modified in `main.cpp`

------------------
# Installation instructions
Note: If `make` fails, change between `$(CXX)` and `mpiccxx` in Makefile. Which one to use depends on installation process, but only one Makefile is included.  

Download and extract UPC++ 1.0 from https://bitbucket.org/upcxx/upcxx/downloads/upcxx-2017.9.0.tar.gz  
After extraction do  
`$ cd upcxx-2017.9.0`  
and proceed to instructions for your environment below:

------------------

## Single node
`$ ./install /upcxx/installation`  
`$ export UPCXX_INSTALL=/upcxx/installation`  
Go to directory of either `upcxx-partitioned-map` or `upcxx-wordcount`  
`$ make`  
`$ $UPCXX_INSTALL/bin/upcxx-run 4 ./kv`    

------------------

## Stallo
### Installation:  
`$ module load OpenMPI/2.1.1-GCC-6.4.0-2.28`  
`$ CC=mpicc CXX=mpicxx ./install /upcxx/installation`  
`$ export UPCXX_INSTALL=/upcxx/installation`

### Compilation:
`$ export UPCXX_GASNET_CONDUIT=ibv`  
Go to directory of either `upcxx-partitioned-map` or `upcxx-wordcount`  
`$ make`

### Running
`$ export GASNET_PHYSMEM_MAX='700 MB'`  
`$ salloc --nodes=4 $UPCXX_INSTALL/bin/upcxx-run 4 ./kv`  

Modifying memory allocation is possible similarly to in Abel instructions below. Unlike Abel, this is not required on Stallo, but should be done if `GASNET_PHYSMEM_MAX` is increased

------------------
## Abel
### Installation on Abel:
`$ module purge`  
`$ module load python2/2.7.10.gnu`  
`$ module load gcc/7.2.0`  
`$ module load openmpi.gnu/2.1.0`  
`$ ./install /upcxx/installation`  

### Compilation:
`$ export UPCXX_GASNET_CONDUIT=ibv`  
`$ export UPCXX_INSTALL=/upcxx/installation`  
`$ export GASNET_PHYSMEM_MAX='8000 MB' `  
Go to directory of either `upcxx-partitioned-map` or `upcxx-wordcount`  
`$ make `    

### Running
`$ salloc --nodes=8 --mem-per-cpu=8000 --time=01:00:00 --account=nn9342k $UPCXX_INSTALL/bin/upcxx-run 8 ./kv `  

Make sure that the number of nodes allocated is equal to the argument of upcxx-run
Find <ACCOUNT> by running the `cost` command

------------------
