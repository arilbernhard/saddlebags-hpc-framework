Saddlebags
=======
Saddlebags is a high performance computing framework, created at the University of Tromsø. It is implemented in C++ with UPC++, and is inspired by big data systems and graph analytics frameworks.

Comprehensive information about Saddlebags can be found in the `doc` directory of this repository.

Example
=======
Saddlebags utilize inheritance and template programming to allow expression of specialized problems with general communication methods, data distribution, and synchronization. Programmers will define Item classes with pre-defined event-triggered methods, with physical location of data being completely transparent.

A Item class for implementing the PageRank algorithm can be defined as follows:

```
template<class Tk, class Ok, class Mt>
class SiteObject : public saddlebags::Item<Tk, Ok, Mt> {
    public:
    float page_rank = 1;
    float new_page_rank = 0;
    std::vector<int> links;

    void add_link(int new_link) {
        this->links.push_back(new_link);
    }

    void do_work() override {
        for(auto it : this->links) {
            this->push(0, it, page_rank/((float)links.size()));
        }
    }

    void foreign_push(Mt val) override {
        new_page_rank += val;
    }

    void finishing_work() override {
        page_rank = new_page_rank;
        new_page_rank = 0;
    }
};
```

Installation and usage
=======

All compute clusters with InfiniBand or Ethernet connections that support MPI is expected to work, but this text will focus on major Norwegian HPC infrastructure, namely the Abel cluster at University of Oslo, and the Stallo cluster at the University of Tromsø. Usage and installation on clusters differ from single node environments in potential workload managers. The following instructions are expected to be relevant for all clusters with the Slurm workload manager and/or Lmod environment module system. Note that different module naming schemes and workload scheduling policies may apply to different clusters, even if its software is similar.

Installation instructions are available in the UPC++ implementation tarball, and are also included here to provide specific instructions for modules and workload managers found on the aforementioned clusters. An overview of the included Saddlebags applications is provided in a readme file in the source code.

Saddlebags has been implemented and tested with UPC++ v2017.9.0, which can be found at https://bitbucket.org/upcxx/upcxx/downloads/upcxx-2017.9.0.tar.gz  
All UPC++ versions, and more information about UPC++, is found at https://bitbucket.org/berkeleylab/upcxx/wiki/Home
## Linux installation
Unzip the tarball, and run the install file, with installation directory as argument:  
`$ ./install /desired/installation/directory`  
Store the installation directory in an environment variable  
`$ export UPCXX_INSTALL=/desired/installation/directory`
## Running UPC++ programs on Linux

First, set a memory limit per node.  
`$ export GASNET_PHYSMEM_MAX='8000MB'`  

There are two ways of running a UPC++ programs in a non-distributed environment. The first method involves setting the number of partitions in an environment variable, and running the program normally.  
`$ cd upcxx-2018.1.0/example/prog-guide`  
`$ make`  
`$ export GASNET_PSHM_NODES=4`  
`$ ./hello-world`  

Another method is using the `upcxx_run` script in the install location, with the number of partitions as argument.  
`$ $UPCXX_INSTALL/bin/upcxx-run 4 hello-world`  

The default communication conduit is detected during installation. It can also be set manually, as follows:  
InfiniBand (clusters):  
`$ export UPCXX_GASNET_CONDUIT=ibv`  
Shared memory (single node):   
`$ export UPCXX_GASNET_CONDUIT=smp`  
MPI:  
`$ export UPCXX_GASNET_CONDUIT=mpi`  

Saddlebags applications are compiled with the `make` command. Note that for Abel, the `mpicxx` C++ compiler must be used.
## Abel installation and usage
Abel uses the Lmod environment module system. Before installation can be performed as in the previous section, the proper modules must be loaded.  
`$ module purge`  
`$ module load python2/2.7.10.gnu`  
`$ module load gcc/7.2.0`  
`$ module load openmpi.gnu/2.1.0`  
First, any pre-loaded modules are removed to ensure that the intended compiler is used. MPI is needed for communication between nodes, and thread spawning. Python is needed for installation 

With all modules loaded, follow the normal Linux installation instructions found earlier in this section. Programs are compiled as shown in section \ref{section:runlinux}.

In order to run workloads on Abel, you need allocated CPU hours on a specific account name. To find your account name:  
`$ cost`  
Run programs with the `salloc` command. Memory usage and max compute time must be provided:  
`$ salloc --nodes=16 --mem-per-cpu=8000 --time=01:00:00 --account=ACCNAME $UPCXX_INSTALL/bin/upcxx-run 16 ./hello-world`  

## Stallo installation and usage
Stallo uses the same module system as Abel, but provides different default modules and names. Here, `mpicxx` can be set as the default C++ compiler during installation:
`$ module load OpenMPI/2.1.1-GCC-6.4.0-2.28`   
`$ CC=mpicc CXX=mpicxx ./install /desired/installation/directory`  
`$ export UPCXX\_INSTALL=/desired/installation/directory`  

Running programs on Stallo is similar as on Abel, but less information is required to queue workloads.  
`$ salloc --nodes=16 \$UPCXX\_INSTALL/bin/upcxx-run 16 ./hello-world`  
Information about memory usage per node is not required, but should be added as an argument, like on Abel, if `GASNET_PHYSMEM_MAX` is set above 1 GB.
