cityhashopath = ../include/cityhash/src/city.o
EXTRA_FLAGS=-std=c++11 -Wno-inline
CXX=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta CXX)
PPFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta PPFLAGS) $(EXTRA_FLAGS)
LDFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta LDFLAGS) $(EXTRA_FLAGS)
LIBFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta LIBFLAGS)

all:
	$(CXX) -c matmul.cpp $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) -g
	$(CXX) -o matmul matmul.o $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) -lm $(cityhashopath) -std=c++17 -g

clean:
	rm -f matmul *.o