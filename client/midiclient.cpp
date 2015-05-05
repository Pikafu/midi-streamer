//*****************************************//
//  midiclient.cpp
//  by John Fu, 2015.
//
//  Simple program to echo MIDI input to 
//  MIDI output.
//
//  compile with: 
//  g++ -g -std=c++11 -Wall -D__LINUX_ALSA__ -o 
//  midiclient midiclient.cpp rtmidi/RtMidi.cpp -lasound -pthread
//
//*****************************************//

/* Standard */
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

/* Threading */
#include <signal.h>
#include <thread>

/* MIDI */
#include "RtMidi.h"

/* Networking */
#include "simple_client.h"

/* DEFS */
#define PORT "3490" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once

// Platform-dependent sleep routines.
#if defined(__WINDOWS_MM__)
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

// Interrupt handler logic
bool done;
static void finish(int ignore){ done = true; }

// Number of client threads
static const int NUM_THREADS = 10;

void usage( void ) {
	// Error function in case of incorrect command-line
	// argument specifications.
	std::cout << "\n message: cmidiin <port>\n";
	std::cout << "    where port = the device to use (default = 0).\n\n";
	exit( 0 );
}

/**
  Prompts user to select input and output midi ports
 */
bool chooseMidiPort( RtMidiIn *rtmidiin, RtMidiOut *rtmidiout )
{
	std::cout << "\nWould you like to open a virtual input port? [y/N] ";
	std::string keyHit;
	std::getline( std::cin, keyHit );
	if ( keyHit == "y" ) {
		rtmidiin->openVirtualPort();
		return true;
	}
	std::cout << "\nDetecting available ports...\n";
	std::string inPortName;
	std::string outPortName;
	unsigned int i = 0, nInPorts = rtmidiin->getPortCount();
	unsigned int j = 0, nOutPorts = rtmidiout->getPortCount();
	if ( nInPorts == 0 ) {
		std::cout << "Input port is not available!" << std::endl;
		return false;
	}
	if ( nOutPorts == 0 ) {
		std::cout << "Output port is not available!" << std::endl;
	}
	if ( nInPorts == 1 ) {
		std::cout << "Opening as input port: " << rtmidiin->getPortName() << std::endl;
	}
	if ( nOutPorts == 1 ) {
		std::cout << "Opening as output port: " << rtmidiout->getPortName() << std::endl;
	}
	else {
		std::cout << "\nChoose from available ports:\n";
		for ( i=0; i<nInPorts; i++ ) {
			inPortName = rtmidiin->getPortName(i);
			std::cout << "  Input port #" << i << ": " << inPortName << '\n';
		}
		for ( j=0; j<nOutPorts; j++ ) {
			outPortName = rtmidiout->getPortName(j);
			std::cout << "  Output port #" << j << ": " << outPortName << '\n';
		}
		do {
			std::cout << "\nChoose number for input port: ";
			std::cin >> i;
		} while ( i >= nInPorts );
		do {
			std::cout << "Choose number for output port: ";
			std::cin >> j;
		} while ( j >= nOutPorts );
		std::getline( std::cin, keyHit );  // used to clear out stdin
	}
	rtmidiin->openPort( i );
	rtmidiout->openPort( j );
	return true;
}

/**
  Receives MIDI from midiin and transmits it out of midiout.
  Shift is the number of semitones you want the output to be.
 */
void echo( RtMidiIn *midiin, RtMidiOut *midiout, int shift )
{
	// Gather incoming MIDI note and generate output MIDI note of the same size
	std::vector<unsigned char> input;
	unsigned int stamp = midiin->getMessage( &input );
	unsigned int nBytes = input.size();
	std::vector<unsigned char> output(nBytes);

	// Shift the tones
	for ( unsigned int i=0; i<nBytes; i++ ) {
		if (i == 1)
			output[i] = (int)input[i] + shift;
		else
			output[i] = input[i];
		std::cout << (int)input[i] << ", ";
		std::cout << (int)output[i] << ", ";
	}
	if ( nBytes > 0 ) {
		std::cout << "stamp = " << stamp << std::endl;
	}
	midiout->sendMessage( &output );
}

// This function will be called from a thread
void call_from_thread(int tid) {
	std::cout << "Launched by thread " << tid << std::endl;
}

int main( int argc, char *argv[] )
{
	// MIDI ports
	RtMidiIn *midiin = 0;
	RtMidiOut *midiout = 0;

	// Store data received from the server
	int server_sockfd = 0;
	std::string rcvdata;

	// Minimal command-line check.
	if ( argc > 2 ) usage();

	try {
		// This function should be embedded in a try/catch block in case of
		// an exception.  It offers the user a choice of MIDI ports to open.
		// It returns false if there are no ports available.
		bool chooseMidiPort( RtMidiIn *rtmidi, RtMidiOut *rtmidiout );

		// RtMidiIn and RtMidiOut constructors
		midiin = new RtMidiIn();
		midiout = new RtMidiOut();

		// Call function to select port.
		if ( chooseMidiPort( midiin, midiout ) == false ) goto clean_up;

		// Don't ignore sysex, timing, or active sensing messages.
		//midiin->ignoreTypes( false, false, false );

		// Install an interrupt handler function.
		done = false;
		(void) signal(SIGINT, finish);

		// Periodically check input queue.
		std::cout << "\nReading MIDI from port ... quit with Ctrl-C.\n";

		//    // Create 2 threads, one for input, one for output
		//    std::thread t_array[2];
		//
		//    for (int i = 0; i < NUM_THREADS; i++) {
		//    	std::thread(call_from_thread, i);
		//    }

		// Connect to the server
		server_sockfd = connect_to_server(argc, argv);

		// Echo input to output
		while ( !done ) {
//			echo( midiin, midiout, 4);
			std::cout << "Got into loop" << std::endl;
			std::string teststr = "shit";
			send_to_server(teststr, server_sockfd);
			rcvdata = recv_from_server(server_sockfd);
			if (!rcvdata.empty()) {
				std::cout << "client: received " << rcvdata << std::endl;
			}
//			buf = recv_from_server(server_sockfd);
//			std::cout << buf << std::endl;
		}

	} catch ( RtMidiError &error ) {
		error.printMessage();
	}

	clean_up:
		std::cout << "\nCleaning Up.\n";
		cleanup(server_sockfd);
		delete midiin;
		delete midiout;
		return 0;
}


