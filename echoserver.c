#include "server.h"

#define MAXLINE 8912

void echo(int connfd) {
	char buf[MAXLINE];
	char msg[MAXLINE] = "world!\n";
	

	recv(connfd, buf, MAXLINE, 0);
	sleep(2);
	send(connfd, msg, MAXLINE, 0);

//	while ((n = recv(connfd, buf, MAXLINE, 0)) != 0) {
//		printf("server received %d bytes\n", n);
//		send(connfd, msg, MAXLINE, 0);
//	}

}

void *producer(void *vargp) {
	int connfd = *((int *)vargp);
	pthread_t tid = pthread_self();
	pthread_detach(tid);
	free(vargp);
	echo(connfd);

	printf("thread <%ld> is running!\n", tid);

	close(connfd);
	return NULL;
}





int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	//variable declaration
	int listenfd;
	int *connfdp;
	int port;
	char *haddrp;
	socklen_t clientlen;
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	pthread_t tid;

	port = atoi(argv[1]);
	clientlen = sizeof(clientaddr);

	listenfd = open_listenfd(port);

	while (1) {
		connfdp = (int *)malloc(sizeof(int));
		*connfdp = accept(listenfd, (SA *) &clientaddr, &clientlen);

		//determine the domainname and IP address of the client
		hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);
		printf("server connected to %s (%s)\n", hp->h_name, haddrp);

		pthread_create(&tid, NULL, producer, connfdp);
		printf("main created thread <%ld>\n", tid);

	}

	return 0;
}



