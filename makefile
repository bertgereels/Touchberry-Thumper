# The compiler to use
CC=g++

# Compiler flags
CFLAGS=-c -Wall -std=c++11
    # -c: Compile or assemble the source files, but do not link. The linking stage simply is not done. The ultimate $
    # -Wall: This enables all the warnings about constructions that some users consider questionable, and that are e$

# Name of executable output
EXECUTABLE=Thumper

all: $(EXECUTABLE)

$(EXECUTABLE): main.o
	$(CC) main.o -o $(EXECUTABLE)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -f *.o $(EXECUTABLE)



