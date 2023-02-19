CC=g++
CFLAGS= -Wall -g

main: main.o Exchange.o Request.o Trader.o 
	$(CC) -c main.cpp Request.cpp Exchange.cpp Trader.cpp
	$(CC) -o main main.o Exchange.o Request.o Trader.o 

