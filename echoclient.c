#include <unistd.h>

#include "client.h"

#define MAXLINE 8192

int main(int argc, char **argv) {

	int clientfd;
	int	port;
	char *host;
    char buf[MAXLINE] = "hello";
    char rcvMsg[MAXLINE];

	if (argc != 3) {
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(0);
	}
	host = argv[1];
	port = atoi(argv[2]);

	
//	while (fgets(buf, MAXLINE, stdin) != NULL) {
//
//		if (strcmp(buf, "exit") == 0) {
//			break;
//		}
//		if (send(clientfd, buf, strlen(buf), 0) < 0) {
//			printf("client send() failed!\n");
//		}
//
//		if (recv(clientfd, rcvMsg, MAXLINE, 0) < 0) {
//			printf("client recv() failed!\n");
//		}
//
//		fputs(rcvMsg, stdout);

//	}

	int n = 3;

	for (int i = 0; i < n; i++) {

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

//	char hostname[20];
//	gethostname(hostname, sizeof(hostname));
//	fprintf(stderr, "hostname is %s\n", hostname);

	return 0;

}



