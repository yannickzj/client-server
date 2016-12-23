#include "client.h"
#include <semaphore.h>
#include <queue>
#include <iostream>
#include <fstream>
#include <string>

#define MAXLINE 1024
#define BUFFER_SIZE 20
#define END_STR "#end"

//'available' semaphore counting the number of available slots in the buffer
//'occupied' semaphore counting the number of occupied slots in the buffer
sem_t available;
sem_t occupied;

//mutex to control the access of the buffer
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//client parameter structure
struct client_parms {
	char* host;
	int port;
	queue<char *> *buf;

};

//client function
void* client(void* parameters) {

	//get input parameters
	struct client_parms* args = (struct client_parms*) parameters;
	char *host = args->host;
	int port = args->port;
	queue<char*> *buf = args->buf;
	pthread_t tid = pthread_self();
	int clientfd;

	string request = "";
	char *message;

	while(1) {
		//get request from the buffer
		sem_wait(&occupied);

		//lock to access the buffer_raw
		pthread_mutex_lock(&lock);

		message = buf->front();
		buf->pop();

		//unlock the mutex on the buffer_raw
		pthread_mutex_unlock(&lock);

		sem_post(&available);

		//if client thread receive end signal, it stop to wait for the buffer
		request = message;
		if (request == END_STR) {
			free(message);
			break;
		}

		//client connect the server
		char rcvMsg[MAXLINE];
		clientfd = open_clientfd(host, port);

		//client sends a request to server
		if (send(clientfd, message, strlen(message)+1, 0) < 0) {
			printf("client <%ld> send() failed!\n", tid);
		}
	
		printf("client <%ld> sent a request: %s\n", tid, message);
	
		//client waits for the reponse of server
		if (recv(clientfd, rcvMsg, MAXLINE, 0) < 0) {
			printf("client <%ld> recv() failed!\n", tid);
		}
		printf("client <%ld> received \"%s\"\n", tid, rcvMsg);

		free(message);

		close(clientfd);
	
	}

	return NULL;

}


int main(int argc, char **argv) {

	if (argc != 4) {
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(0);
	}
	char *host = argv[1];
	int port = atoi(argv[2]);
	int numClient = atoi(argv[3]);

	//create two buffers
	queue<char *> buf;	

	//define client parameters
	pthread_t list_c[numClient];
	struct client_parms client_args;
	client_args.host = host;
	client_args.port = port;
	client_args.buf = &buf;

	//semaphore initialization
	sem_init (&available, 0, BUFFER_SIZE);
	sem_init (&occupied, 0, 0);
	
	//begin client threads in parallel
	for (int i=0; i<numClient; i++) {
		if (pthread_create (&list_c[i], NULL, &client, &client_args)!=0) {
			fprintf(stderr, "client pthread_create error!\n");
		}
	}


	//read input file and send request lines in the buffer
	char *filename = (char *)"./input/requests.txt";
	ifstream infile;
	infile.open(filename);

	if (!infile.good()) {
		cout << "client main connot open the input file!" << endl;
	}

	char lineBuf[MAXLINE];
	infile.getline(lineBuf, MAXLINE);
	string s = lineBuf;
	
	while(!infile.eof()) {
	
		if (!s.empty()) {
			int length = s.size();
			char *request = new char[length+1];
			strcpy(request, s.c_str());

			//get request from the buffer
			sem_wait(&available);

			//lock to access the buffer
			pthread_mutex_lock(&lock);

			buf.push(request);

			//unlock the mutex on the buffer
			pthread_mutex_unlock(&lock);

			sem_post(&occupied);
		}

		infile.getline(lineBuf, MAXLINE);
		s = lineBuf;
	}

	infile.close();

	//send end signals in the buffer
	string end = END_STR;
	int endLength = end.size();
	for (int i = 0; i < numClient; i++) {
		char *request = new char[endLength+1];
		strcpy(request, end.c_str());

		sem_wait(&available);

		//lock to access the buffer
		pthread_mutex_lock(&lock);

		buf.push(request);

		//unlock the mutex on the buffer
		pthread_mutex_unlock(&lock);

		sem_post(&occupied);

	}

	//join the producer and consumer threads
	for (int i = 0; i < numClient; i++) {
		pthread_join (list_c[i], NULL);
	}

	return 0;
}
