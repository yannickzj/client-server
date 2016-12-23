#include "server.h"
#include "graph.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXLINE 1024
#define C 5
#define P 5
#define MSGEND '!'
#define BUFFER_SIZE_RAW 20
#define BUFFER_SIZE_PARSED 20
#define INT_STR_LENGTH 8

using namespace std;

//semaphores to coodinate producers and consumers 
//sem_t sem_pi;
//sem_t sem_po;
//sem_t sem_c;

//'available_raw' and 'available_parsed' semaphore counting the number of available slots in the two buffers 
//'occupied_raw' and 'occupied_parsed' semaphore counting the number of occupied slots in the two buffers 
sem_t available_raw;
sem_t occupied_raw;
sem_t available_parsed;
sem_t occupied_parsed;

//mutex to control the access of the two buffers
pthread_mutex_t lock_raw = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_parsed = PTHREAD_MUTEX_INITIALIZER;

//request structure
struct request_raw {
	int connfd;
	char *message;
};

//parsed request structure
struct request_parsed {
	int connfd;
	char **args;
};

//producer parameter structure
struct producer_parms {
	//int connfd;
	queue<struct request_raw> *bufIn;
	queue<struct request_parsed> *bufOut;
};

//consumer parameter structure
struct consumer_parms {
	queue<struct request_parsed> *bufIn;
	Graph* map;     
};


//string split function
vector<string> split(string str, string pattern) {  //  split function
	string::size_type start = 0;
	string::size_type end;
	vector<string> result;
	str += pattern;
	string::size_type size = str.size();

	while (start < size) {
		end = str.find(pattern, start);
		if (end > start) {
			string substr = str.substr(start, end - start);
			result.push_back(substr);
		}
		start = end + pattern.size();
	}

	return result;
}

//message parsing function
char** parse(char* message) {

	string str = message;
	vector<string> strSplit = split(str, " ");
	string key = strSplit[0];
	cout << "key = " << key << endl;

	if (key != "trip" && key != "add") {
		cout << "no key in message!" << endl;
		return NULL;
	}

	char **args = new char*[strSplit.size()+1];
	char *arg0 = new char[INT_STR_LENGTH];
	snprintf(arg0, INT_STR_LENGTH, "%d", strSplit.size());
	args[0] = arg0;

	for (int i = 1; i <= strSplit.size(); i++) {
		int length = strSplit[i-1].size();
		char *arg = new char[length+1];
		const char *temp = strSplit[i-1].c_str();
		strcpy(arg, temp);
		args[i] = arg;
	}

	free(message);
	return args;
}

char* process(char **args, Graph *map) {





	char *result = (char *)"message processed!";


	int num_args = atoi(args[0])+1;
	for (int i = 0; i < num_args; i++) {
		//cout << "free:" << args[i] << ";  ";
		free(args[i]);
	}
	
	free(args);

	return result;

}


void *producer(void *parameters) {
	//get parameters
	struct producer_parms *args = (struct producer_parms *) parameters;
	queue<struct request_raw> *bufIn = args->bufIn;
	queue<struct request_parsed> *bufOut = args->bufOut;

	pthread_t tid = pthread_self();
	//pthread_detach(tid);
	//free(args);

	//char message[MAXLINE];
	//char msg[MAXLINE] = "world!\n";

	//recv(connfd, message, MAXLINE, 0);
	//string str = message;


	struct request_raw rq_raw;
	struct request_parsed rq_parsed;

	while(1) {
		//get request from the buffer_raw
		//sem_wait(&sem_pi);

		sem_wait(&occupied_raw);

		//lock to access the buffer_raw
		pthread_mutex_lock(&lock_raw);

		rq_raw = bufIn->front();
		bufIn->pop();

		//unlock the mutex on the buffer_raw
		pthread_mutex_unlock(&lock_raw);


		sem_post(&available_raw);
	
		//sem_post(&sem_pi);
	




		//parse and valid the raw message
		int connfd = rq_raw.connfd;
		char* message = rq_raw.message;
		//printf("producer<%ld> connfd = %d, message = %s\n",tid,  connfd, message);

		char** args = parse(message);
		if (args == NULL) {
			//reply to client
			char msg[] = "It is not a valid request!";
			send(connfd, msg, sizeof(msg), 0);
			close(connfd);
		}
	




		//send the valid message into the buffer_parsed
		rq_parsed.connfd = connfd;
		rq_parsed.args = args;

		//sem_wait(&sem_po);

		sem_wait(&available_parsed);

		//lock to access the buffer_parsed
		pthread_mutex_lock(&lock_parsed);

		bufOut->push(rq_parsed);

		//unlock the mutex on the buffer_parsed
		pthread_mutex_unlock(&lock_parsed);


		sem_post(&occupied_parsed);
	
		//sem_post(&sem_po);
	
	}

	return NULL;
}


void *consumer(void *parameters) {
	//get parameters
	struct consumer_parms *args = (struct consumer_parms *) parameters;
	queue<struct request_parsed> *bufIn = args->bufIn;
	Graph* map = args->map;

	struct request_parsed rq_parsed;

	while(1) {
		//get request from the buffer_raw
		sem_wait(&occupied_parsed);

		//lock to access the buffer_raw
		pthread_mutex_lock(&lock_parsed);

		rq_parsed = bufIn->front();
		bufIn->pop();

		//unlock the mutex on the buffer_raw
		pthread_mutex_unlock(&lock_parsed);

		sem_post(&available_parsed);
	
	
		//process the parsed request
		int connfd = rq_parsed.connfd;
		char** args = rq_parsed.args;
		
		char* result = process(args, map);
	
		send(connfd, result, strlen(result)+1, 0);
		close(connfd);

		}


	return NULL;
}



int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	//variable declaration
	int listenfd;
	int connfd;
	int port;
	char *haddrp;
	socklen_t clientlen;
	struct sockaddr_in clientaddr;
	struct hostent *hp;
	pthread_t tid;

	port = atoi(argv[1]);
	clientlen = sizeof(clientaddr);

	listenfd = open_listenfd(port);


	//create two buffers
	queue<struct request_raw> buf_raw;	
	queue<struct request_parsed> buf_parsed;	

	Graph m1;
//	m1.retrieve("./resources/test1.txt");
//	m1.vertex("ccjil");
//	m1.edgeEvent("E26",CLOSE);
//	m1.trip("CLV", "340", "path1", SHORTEST);
//	m1.edgeEvent("E26",OPEN);
//	m1.trip("CLV", "340", "path1", FASTEST);
//	m1.addVertex("RCH",POINT_OF_INTEREST,4,5);
//	m1.addEdge("additional","RCH","CLV",V1_TO_V2,50,2,OPEN);
//	m1.vertex("CLN");
//	m1.store("output1.txt");

	//semaphore initialization
	sem_init (&available_raw, 0, BUFFER_SIZE_RAW);
	sem_init (&occupied_raw, 0, 0);
	sem_init (&available_parsed, 0, BUFFER_SIZE_PARSED);
	sem_init (&occupied_parsed, 0, 0);

	//thread parameter initialization
	pthread_t list_p[P];
	pthread_t list_c[C];

	struct producer_parms producer_args;
	producer_args.bufIn = &buf_raw;
	producer_args.bufOut = &buf_parsed;

	struct consumer_parms consumer_args;
	consumer_args.map = &m1;
	consumer_args.bufIn = &buf_parsed;


	//begin the producer and consumer threads in parallel 
	int i;
	for (i=0; i<P; i++) {
		if (pthread_create(&list_p[i], NULL, &producer, &producer_args)!=0) {
			fprintf(stderr, "producer pthread_create error!\n");
		}
	}
	for (i=0; i<C; i++) {
		if (pthread_create(&list_c[i], NULL, &consumer, &consumer_args)!=0) {
			fprintf(stderr, "consumer pthread_create error!\n");
		}
	}



	struct request_raw request_raw;
	struct request_raw test;
	while (1) {
		//producer_argsp = (struct producer_parms *)malloc(sizeof(struct producer_parms));

		connfd = accept(listenfd, (SA *) &clientaddr, &clientlen);

//		//determine the domainname and IP address of the client
//		hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
//		haddrp = inet_ntoa(clientaddr.sin_addr);
//		printf("server connected to %s (%s)\n", hp->h_name, haddrp);


		char *message = new char[MAXLINE];
		
		recv(connfd, message, MAXLINE, 0);

		request_raw.connfd = connfd;
		request_raw.message = message;

		sem_wait(&available_raw);

		//lock
		pthread_mutex_lock(&lock_raw);
		

		buf_raw.push(request_raw);
		test = buf_raw.front();
		
		//unlock
		pthread_mutex_unlock(&lock_raw);

		sem_post(&occupied_raw);
		string str = test.message;
		//printf("buf_raw front connfd = %d, message = %s\n", test.connfd, str.c_str());


		//pthread_create(&tid, NULL, producer, producer_argsp);

	}

	return 0;
}



