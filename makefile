CC = gcc
GG = g++
FLAGS= -pthread

all: server client guard singleton selectclient pollserver

server: server.o main1.o active_object.o
	$(CC) server.o main1.o active_object.o -o server $(FLAGS)

client: client.o
	$(CC) $< -o client

guard:
	$(GG) guard.cpp -o guard $(FLAGS)

singleton:
	$(GG) singleton.cpp -o singleton $(FLAGS)

pollserver: pollserver.o
	$(CC) $<  -o pollserver $(FLAGS)

selectclient: selectclient.o
	$(CC) $<  -o selectclient $(FLAGS)

clean:
	rm -f *.o server client main1 active_object guard singleton pollserver selectclient

.PHONY: all run clean
