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

#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "RtMidi.h"

// Platform-dependent sleep routines.
#if defined(__WINDOWS_MM__)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

// Used for polling
bool done;
static void finish(int ignore){ done = true; }

void usage( void ) {
  // Error function in case of incorrect command-line
  // argument specifications.
  std::cout << "\nuseage: cmidiin <port>\n";
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
  Receives midi from midiin and transmits it out of midiout.
 */
void echo( RtMidiIn *midiin, RtMidiOut *midiout )
{
  int nBytes, i;
  std::vector<unsigned char> input, output;
  double stamp;
  stamp = midiin->getMessage( &input );
  nBytes = input.size();
  for ( i=0; i<nBytes; i++ )
    std::cout << (int)input[i] << ", ";
  if ( nBytes > 0 )
    std::cout << "stamp = " << stamp << std::endl;
  output = input;
  SLEEP( 100 );
  midiout->sendMessage( &output );
}

int main( int argc, char ** /*argv[]*/ )
{
  RtMidiIn *midiin = 0;
  RtMidiOut *midiout = 0;

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
    if ( chooseMidiPort( midiin, midiout ) == false ) goto cleanup;

    // Don't ignore sysex, timing, or active sensing messages.
    // midiin->ignoreTypes( false, false, false );
    
    // Install an interrupt handler function.
    done = false;
    (void) signal(SIGINT, finish);
    
    // Periodically check input queue.
    std::cout << "\nReading MIDI from port ... quit with Ctrl-C.\n";
    while ( !done ) {
      echo( midiin, midiout );
    }
  } catch ( RtMidiError &error ) {
    error.printMessage();
  }

  cleanup:
    std::cout << "\nCleaning Up.\n";
    delete midiin;
    return 0;
}


