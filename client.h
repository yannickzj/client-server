#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <string>

#define MAXLINE 8192
#define NUM_REQUEST 1

using namespace std;

typedef struct sockaddr SA;

//consumer parameter structure
struct client_parms {
	char* host;
	int port;
};


int open_clientfd(char *hostname, int port) {
	int clientfd;
	struct hostent *hp;
	struct sockaddr_in serveraddr;

	clientfd = socket(AF_INET, SOCK_STREAM, 0);
	if (clientfd < 0) {
		printf("client could not create socket!\n");
		return -1;
	}

	//fill in the server's IP address and port
	hp = gethostbyname(hostname);
	if (hp == NULL) {
		printf("client failed to get host by name!\n");
		return -2;
	}
	
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)hp->h_addr_list[0], (char *) &serveraddr.sin_addr.s_addr, hp->h_length);
	serveraddr.sin_port = htons(port);

	//printf("serveraddr = %s, port = %d\n", inet_ntoa(serveraddr.sin_addr), serveraddr.sin_port);
	
	//establish a connection with the server
	if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0) {
		printf("client connect failed!\n");
		return -1;
	}

	return clientfd;

}


void* client(void* parameters) {

	//get input parameters
	struct client_parms* args = (struct client_parms*) parameters;
	char *host = args->host;
	int port = args->port;
	

	int clientfd;
	char buf[MAXLINE] = "hello";
    char rcvMsg[MAXLINE];

	for (int i = 0; i < NUM_REQUEST; i++) {


		clientfd = open_clientfd(host, port);

		if (send(clientfd, buf, strlen(buf), 0) < 0) {
			printf("client send() failed!\n");
		}
	
		printf("client sent a request: %s\n", buf);
	
		if (recv(clientfd, rcvMsg, MAXLINE, 0) < 0) {
			printf("client recv() failed!\n");
		}
		printf("received: %s\n", rcvMsg);

		close(clientfd);

	}

	return NULL;

}






