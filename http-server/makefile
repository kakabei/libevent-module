CXX = g++
RM = rm -f
PROTOC = ../ext/protobuf-2.5.0/src/protoc --cpp_out=.
CXXFLAGS += -Wall -D_GNU_SOURCE  -g -DLINUX -O2 -fPIC
INCS = -I. -I../include 
LIBS =  ../lib/libevent.a -lrt

TARGETS = http-server

all:$(TARGETS)

http-server:server_proc.o server_main.o
	$(CXX) $^ $(LIBS) -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCS) $< -o $@

%.pb.o: %.pb.cc
	$(CXX) -c $(CXXFLAGS) $(INCS) $< -o $@

%.pb.cc: %.proto
	$(PROTOC) $<

clean:
	$(RM) $(TARGETS) *.o *.exe
