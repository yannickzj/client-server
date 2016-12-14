#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define LISTENQ 1024

typedef struct sockaddr SA;

int open_listenfd(int port) {
	int listenfd;
    int	optval = 1;
	struct sockaddr_in serveraddr;

	//create a socket descriptor
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
		printf("server could not create socket!\n");
		return -1;
	}

	//eliminates "address already in use" error from bind
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(int)) < 0) {
		return -1;
	}

	//listerfd will be an end point for all requests to port on any IP address for this host
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	//serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons((unsigned short)port);
	
	//printf("serveraddr = %s, port = %d\n", inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));

	if (bind(listenfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0) {
		printf("server could not bind socket!\n");
		return -1;
	}

	//make it a listening socket ready to accept connection requests
	if (listen(listenfd, LISTENQ) < 0) {
		printf("server could not listen socket!\n");
		return -1;
	}


	return listenfd;

}




