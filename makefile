# Compiler
CC = g++

# Standard Flags
CFLAGS = -std=c++11 -Wall -D__LINUX_ALSA__ -pthread

# Dependencies
CLIENT_DPS = ./rtmidi/RtMidi.cpp ./client/simple_client.cpp

# Libraries
CLIENT_LIBS = -lasound -lpthread
SERVER_LIBS = -lpthread

# Output Directory
OUT_DIR = ./bin/

all: midiclient simple_server

midiclient:
	$(CC) $(CFLAGS) ./client/midiclient.cpp $(CLIENT_DPS)	$(CLIENT_LIBS) -o $(OUT_DIR)midiclient
	
simple_server:
	$(CC) $(CFLAGS) ./server/simple_server.cpp	$(SERVER_LIBS) -o $(OUT_DIR)simple_server
	
clean:
	rm ./client/midiclient.o
	rm ./server/simple_server.o
		
