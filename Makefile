CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

OBJS = main.o parser.o node.o testTree.o scanner.o

all: parser

parser: $(OBJS)
	$(CXX) $(CXXFLAGS) -o parser $(OBJS)

main.o: main.cpp parser.h node.h testTree.h scanner.h token.h
	$(CXX) $(CXXFLAGS) -c main.cpp

parser.o: parser.cpp parser.h node.h scanner.h token.h
	$(CXX) $(CXXFLAGS) -c parser.cpp

node.o: node.cpp node.h token.h
	$(CXX) $(CXXFLAGS) -c node.cpp

testTree.o: testTree.cpp testTree.h node.h token.h
	$(CXX) $(CXXFLAGS) -c testTree.cpp

scanner.o: scanner.cpp scanner.h token.h
	$(CXX) $(CXXFLAGS) -c scanner.cpp

clean:
	rm -f *.o parser
