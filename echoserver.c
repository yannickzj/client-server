#include "server.h"

#define MAXLINE 8912

void echo(int connfd) {
	char buf[MAXLINE];
	char msg[MAXLINE] = "world!\n";
	

	recv(connfd, buf, MAXLINE, 0);
	sleep(5);
	send(connfd, msg, MAXLINE, 0);

//	while ((n = recv(connfd, buf, MAXLINE, 0)) != 0) {
//		printf("server received %d bytes\n", n);
//		send(connfd, msg, MAXLINE, 0);
//	}

}

int main(int argc, char **argv) {
	int listenfd;
	int connfd;
	int port;
	char *haddrp;
	socklen_t clientlen;
	struct sockaddr_in clientaddr;
	struct hostent *hp;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	port = atoi(argv[1]);
	//printf("port: %d\n", port);

	listenfd = open_listenfd(port);

	while (1) {
		clientlen = sizeof(clientaddr);
		connfd = accept(listenfd, (SA *) &clientaddr, &clientlen);
		if (connfd < 0) {
			printf("server failed to accept!\n");
			return -1;
		}
		//printf("hello!\n");

		//determine the domainname and IP address of the client
		hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);
		printf("server connected to %s (%s)\n", hp->h_name, haddrp);

		echo(connfd);
		close(connfd);

	}

	return 0;
}



