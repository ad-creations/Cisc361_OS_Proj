# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
#  -Iheaders - this flag links the header files stored in the headers folder into compilation
CFLAGS  = -g -Wall -I../headers
SRCS=main.c utils.c input.c
BIN=../bin
# The build target 

all: main

run: main
	./$(BIN)/main

main: $(SRCS)
	$(CC) $(CFLAGS) -o $(BIN)/main $(SRCS)

clean:
	rm $(BIN)/*.o
	rm $(BIN)/main
