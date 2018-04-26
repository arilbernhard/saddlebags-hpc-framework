EXTRA_FLAGS=-std=c++11 -Wno-inline
CXX=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta CXX)
PPFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta PPFLAGS) $(EXTRA_FLAGS)
LDFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta LDFLAGS) $(EXTRA_FLAGS)
LIBFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta LIBFLAGS)

all:
	$(CXX) -c main.cpp $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) 
	$(CXX) -o main main.o $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) -std=c++17

clean:
	rm -f pagerank *.o