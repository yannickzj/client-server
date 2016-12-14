#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

typedef struct sockaddr SA;

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

	printf("serveraddr = %s, port = %d\n", inet_ntoa(serveraddr.sin_addr), serveraddr.sin_port);
	
	//establish a connection with the server
	if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0) {
		printf("client connect failed!\n");
		return -1;
	}

	return clientfd;

}


