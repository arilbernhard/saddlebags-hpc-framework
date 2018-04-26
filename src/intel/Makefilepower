cityhashopath = ../include/cityhash/src/city.o
EXTRA_FLAGS=-std=c++11 -Wno-inline
CXX=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta CXX)
PPFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta PPFLAGS) $(EXTRA_FLAGS)
LDFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta LDFLAGS) $(EXTRA_FLAGS)
LIBFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta LIBFLAGS)

all:
	$(CXX) -c pagerank.cpp $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) -I/opt/intel-pcm/ -L/opt/intel-pcm/intelpcm.so -lintelpcn -Wl,-rpath /opt/intel-pcm/intelpcm.so
	$(CXX) -o pagerank pagerank.o $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) -lm $(cityhashopath) -std=c++17 -I/opt/intel-pcm/ -L/opt/intel-pcm/intelpcm.so -lintelpcm -Wl,-rpath /opt/intel-pcm/intelpcm.so

clean:
	rm -f pagerank *.o