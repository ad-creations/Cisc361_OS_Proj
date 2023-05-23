CC = gcc
CFLAGS = -g -WALL -I
SRCS = main.c System.c Job.c command.c
BIN = ../BIN

all:  main

run:  main
	./main.exe

main: $(SRCS)
	$(CC) $(CFLAGS) -o main.exe $(SRCS)
clean:
	rm *.o
	rm *.exe