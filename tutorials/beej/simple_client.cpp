//*****************************************//
//  client.cpp (from Beej)
//  by John Fu, 2015.
//
//  Simple program to connect to a host and send hello. 
//  Use like: client "hostname" where "hostname" is the 
//  host name that you're interested in. The address can 
//  be either a host name, like "www.example.com", or an 
//  IPv4 or IPv6 address (passed as a string). This 
//  parameter can also be NULL if you're using the 
//  AI_PASSIVE flag.
//
//  compile with: 
//  g++ -g -std=c++11 -Wall client.cpp -o client
//
//*****************************************//

/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;                   // sockfd - stores socket descriptor, 
                                            // numbytes - stores # bytes read into the buffer from recv()
    char buf[MAXDATASIZE];                  // Store messages from the server
    struct addrinfo hints, *servinfo, *p;   // hints - an addrinfo struct from netdb that we can fill in
                                            // servinfo - linked list of struct addrinfos, set by getaddrinfo()
                                            // p - used in loop, stores the address info of the first server we connect to
    int rv;                                 // Stores the success/failure of getaddrinfo call, 0 if success, nonzero on error
    char s[INET6_ADDRSTRLEN];               // Max length for IPv6 msgs, used in inet_ntop() call

    if (argc != 2) {                        // Must have 2 arguments
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }
    // Set up our addrinfo structure
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // getaddrinfo error handling
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through the linkedlist of addrinfo structs and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        // Error creating a socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        // Error connecting to a socket
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }
    
    // Could not find a socket to connect to
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    // Convert IP address to printable format
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);
    
    freeaddrinfo(servinfo); // free the linkedlist
    
    // Error with recv()
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    
    buf[numbytes] = '\0'; // pad message with 0 (Why?)
    printf("client: received '%s'\n",buf);
    close(sockfd);
    return 0;
}
