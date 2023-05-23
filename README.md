Compile using:

gcc -Wall -g -c main.c job.c command.c system.c

flags are:

-Wall - this flag is used to turn on most comiler warnings
-g - this flag adds debugging info to the executable file

link:

gcc -Wall -o main.exe command.o main.o System.o Job.o

run:
./main.exe


some versions would start to run and add jobs to queue but eventually all got some type of invalid parameter type 0 or 3 or ect.
no fix found, stackoverflow couldn't identify a fix, just myself working on this project for 36 hours debuggin was no help...