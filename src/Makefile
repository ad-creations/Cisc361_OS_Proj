# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
#  -Iheaders - this flag links the header files stored in the headers folder into compilation
CFLAGS  = -g -Wall -I../structures
SRCS=main.cpp utils.cpp input.cpp
BIN=../bin
# The build target 

all: main

run: main
	./$(BIN)/main.exe

main: $(SRCS)
	$(CC) $(CFLAGS) -o $(BIN)/main.exe $(SRCS)

clean:
	rm bin/*.o
	rm bin/*.exe