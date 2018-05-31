This directory contains the source code of Saddlebags.
Comprehensive instructions for installation and usage is found in Appendix A in the thesis.
Note that on Abel and possibly other cluster enviroments, $(CXX) in Makefiles should be replced with mpicxx


Code is organized into the following directories:
include - contains the Saddlebags framework implementation, and CityHash source code (with license to redistribute this code)
text_pagerank - a program that runs PageRank on a simple graph and checks for potential errors in calculation
test_hashtable - a program that inserts intems into and validates the hash table
pagerank - regular PageRank implementation
tfidf - tfidf program - datasets for this program is not included



example - with UPCXX_INSTALL set to installation path of UPC++
$ cd test_pagerank
$ export GASNET_PSHM_NODES=4
$ make
$ ./pagerank
running with 4 ranks
Mode is 3
CYCLE 1
Partition 3 has in total 15 items
Partition 1 has in total 11 items
Partition 2 has in total 16 items
Partition 0 has in total 17 items
CYCLE 2
Partition 3 has in total 15 items
Partition 0 has in total 17 items
Partition 2 has in total 16 items
Partition 1 has in total 11 items
CYCLE 3
Partition 3 has in total 15 items
Partition 0 has in total 17 items
Partition 2 has in total 16 items
Partition 1 has in total 11 items
