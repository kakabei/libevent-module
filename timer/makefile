CXX = g++
RM = rm -f
PROTOC = ../ext/protobuf-2.5.0/src/protoc --cpp_out=.
CXXFLAGS += -Wall -D_GNU_SOURCE  -g -DLINUX -O0 -fPIC
INCS = -I. -I../include -I..
LIBS =  ../lib/libevent.a -lrt
VPATH=.. \
	  ../util/ \


TARGETS = timer-deamon

all:$(TARGETS)

timer-deamon:timer_proc.o ini_file.o systerm_tool.o timer_main.o
	$(CXX) $^ $(LIBS) -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCS) $< -o $@

clean:
	$(RM) $(TARGETS) *.o *.exe
