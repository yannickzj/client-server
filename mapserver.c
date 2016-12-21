#include "server.h"
#include "graph.h"

#define MAXLINE 1024
#define NUM_CONSUMER 5

//consumer parameter structure
struct consumer_parms {
	int connfd;
	Graph* map;     
};


void echo(int connfd) {
	char buf[MAXLINE];
	char msg[MAXLINE] = "world!\n";
	

	recv(connfd, buf, MAXLINE, 0);
	sleep(2);
	send(connfd, msg, MAXLINE, 0);


}

void valid(char *message) {

}



void *producer(void *vargp) {
	int connfd = *((int *)vargp);

	pthread_t tid = pthread_self();
	pthread_detach(tid);
	free(vargp);

	char buf[MAXLINE];
	char msg[MAXLINE] = "world!\n";

	recv(connfd, buf, MAXLINE, 0);

	valid(buf);

	//echo(connfd);

	send(connfd, msg, MAXLINE, 0);

	close(connfd);
	return NULL;
}


void *consumer(void *parameters) {

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

	Graph m1;
	m1.retrieve("./resources/test1.txt");
//	m1.vertex("ccjil");
//	m1.edgeEvent("E26",CLOSE);
//	m1.trip("CLV", "340", "path1", SHORTEST);
//	m1.edgeEvent("E26",OPEN);
//	m1.trip("CLV", "340", "path1", FASTEST);
//	m1.addVertex("RCH",POINT_OF_INTEREST,4,5);
//	m1.addEdge("additional","RCH","CLV",V1_TO_V2,50,2,OPEN);
//	m1.vertex("CLN");
//	m1.store("output1.txt");


	//consumer threads declaration
	int C = NUM_CONSUMER;
	pthread_t list_c[C];

	//begin to transfer data
	int i;
	for (i=0; i<C; i++) {
		if (pthread_create (&list_c[i], NULL, &consumer, &consumer_args)!=0) {
			fprintf(stderr, "consumer pthread_create error!\n");
		}
	}



//	while (1) {
//		connfdp = (int *)malloc(sizeof(int));
//		*connfdp = accept(listenfd, (SA *) &clientaddr, &clientlen);
//
////		//determine the domainname and IP address of the client
////		hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
////		haddrp = inet_ntoa(clientaddr.sin_addr);
////		printf("server connected to %s (%s)\n", hp->h_name, haddrp);
//
//		pthread_create(&tid, NULL, producer, connfdp);
////		printf("main created thread <%ld>\n", tid);
//
//	}

	return 0;
}



