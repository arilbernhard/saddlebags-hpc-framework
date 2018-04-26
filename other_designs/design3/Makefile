cityhashopath = include/cityhash/src/city.o
EXTRA_FLAGS=-std=c++11 -Wno-inline
CXX=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta CXX)
PPFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta PPFLAGS) $(EXTRA_FLAGS)
LDFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta LDFLAGS) $(EXTRA_FLAGS)
LIBFLAGS=$(shell $(UPCXX_INSTALL)/bin/upcxx-meta LIBFLAGS)

all:
	$(CXX) -c include/message.cpp $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) 
	$(CXX) -c include/data_object.cpp $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) 
	$(CXX) -c include/table_container.cpp $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS)
	$(CXX) -c include/worker.cpp $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) 
	$(CXX) -c main.cpp $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) 
	$(CXX) -o main main.o data_object.o table_container.o message.o worker.o $(PPFLAGS) $(LDFLAGS) $(LIBFLAGS) -lm $(cityhashopath) -std=c++17