# Compiler
CC = g++

# Standard Flags
CFLAGS = -std=c++11 -Wall -D__LINUX_ALSA__ -pthread

# Dependencies
CDPS = ./rtmidi/RtMidi.cpp ./client/simple_client.cpp

# Libraries
CLIBS = -lasound -lpthread

# Output Directory
OUT_DIR = ./bin/

all: midiclient

midiclient:
	$(CC) $(CFLAGS) ./client/midiclient.cpp $(CDPS)	$(CLIBS) -o $(OUT_DIR)midiclient
	
clean:
	rm ./client/midiclient.o
		
