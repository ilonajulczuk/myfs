CC=g++
OUTPUT=main
SOURCES=btree.cc bfs.cc
TESTS=btree_test.cc bfs_test.cc

all: main

main: *.cc *.h
	$(CC) -Wall -std=c++11 $(SOURCES) main.cc -o $(OUTPUT)

clean:
	rm $(OUTPUT)

test: *.cc *.h
	$(CC) -Wall -std=c++11 -isystem $(GTEST_DIR)/include -pthread btree_test.cc bfs_test.cc $(SOURCES) $(GTEST_DIR)/libgtest.a -o test

