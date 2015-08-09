CC = g++
CPPFLAGS = -std=c++0x -Wall -O2

OPENCVFLAGS = `pkg-config --cflags --libs opencv`
THRIFTFLAGS = -DHAVE_INTTYPES_H -DHAVE_NETINET_IN_H -lthrift

SOURCES = Algorithm.cc RAlgorithm.cc
OBJECTS = $(SOURCES:.cc=.o)

.PHONY: clean
ALL: make.dep server client a

make.dep: $(SOURCES) main.cc
	$(CC) $(CPPFLAGS) -MM $^ > make.dep

server: gen-cpp/*.cpp gen-cpp/*.h server.cc $(OBJECTS)
	$(CC) -g gen-cpp/*.cpp server.cc $(OBJECTS) $(CPPFLAGS) $(OPENCVFLAGS) $(THRIFTFLAGS) -lboost_serialization -ldl -lpthread -o server

client: gen-cpp/*.cpp gen-cpp/*.h client.cc
	$(CC) -g gen-cpp/*.cpp client.cc $(CPPFLAGS) $(THRIFTFLAGS) -o client

a: $(OBJECTS) main.o
	$(CC) -g $^ $(CPPFLAGS) $(OPENCVFLAGS) -lboost_serialization -ldl -lpthread -o a

include make.dep

clean:
	rm -f *.o a server client make.dep
