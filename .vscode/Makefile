CC = gcc
CFLAGS = -g -WALL -I
SRCS = main.c System.c Job.c command.c
BIN = ../BIN

all:  main

run:  main
	./$(BIN)/main.exe

main: $(SRCS)
	$(CC) $(CFLAGS) -o $(BIN)/main.exe $(SRCS)
clean:
	rm BIN/*.o
	rm BIN/*.exe