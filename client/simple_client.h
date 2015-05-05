/*
 * simple_client.h
 *
 *  Created on: Apr 30, 2015
 *      Author: katahdin
 */

#ifndef SIMPLE_CLIENT_H_
#define SIMPLE_CLIENT_H_

void *get_in_addr(struct sockaddr *sa);
int connect_to_server(int argc, char *argv[]);
std::string recv_from_server(int sockfd);
void send_to_server(std::string data, int sockfd);
void cleanup(int sockfd);

#endif /* SIMPLE_CLIENT_H_ */
