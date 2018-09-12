CC=gcc
CXX=g++
CFLAGS=-g -std=c++11 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -Wall #-DVERBOSE
BIN=../bin

all: uniprot2any


uniprot2any: main.cpp
	$(CXX) $(CFLAGS) main.cpp -o $(BIN)/uniprot2any

clean:
	rm -rf $(BIN)/uniprot2gi
