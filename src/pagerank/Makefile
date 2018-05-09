cityhashopath = ../include/cityhash/src/city.o
EXTRA_FLAGS= -std=c++17 -Wno-inline
CXX=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta CXX)
PPFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta PPFLAGS) $(EXTRA_FLAGS)
LDFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta LDFLAGS) $(EXTRA_FLAGS)
LIBFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta LIBFLAGS)

all:
	$(CXX) -o pagerank pagerank.cpp $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) -lm $(cityhashopath)

clean:
	rm -f pagerank *.o