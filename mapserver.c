#include "server.h"
#include "graph.h"

#define MAXLINE 1024
#define MSGEND '!'
#define BUFFER_SIZE_RAW 20
#define BUFFER_SIZE_PARSED 20
#define INT_STR_LENGTH 8
#define DEFAULT_VERTEXTYPE POINT_OF_INTEREST
#define DEFAULT_X 0
#define DEFAULT_Y 0

using namespace std;

//'available_raw' and 'available_parsed' semaphore counting the number of available slots in the two buffers 
//'occupied_raw' and 'occupied_parsed' semaphore counting the number of occupied slots in the two buffers 
sem_t available_raw;
sem_t occupied_raw;
sem_t available_parsed;
sem_t occupied_parsed;

//mutex to control the access of the two buffers
pthread_mutex_t lock_raw = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_parsed = PTHREAD_MUTEX_INITIALIZER;

//mutex to control the access of the server map
pthread_mutex_t lock_map = PTHREAD_MUTEX_INITIALIZER;

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

	//to check whether the request message is valid
	if (key != "trip" && key != "add") {
		cout << "no key in message!" << endl;
		return NULL;
	}
	if (key == "trip") {
		if (strSplit.size() != 4 || (strSplit[1] != "fastest" && strSplit[1] != "shortest")) {
			cout << "parameters not valid!" << endl;
			return NULL;
		}
	}
	if (key == "add") {
		if (strSplit.size() != 8 || (atoi(strSplit[4].c_str()) != 0 && atoi(strSplit[4].c_str()) != 1 && atoi(strSplit[4].c_str()) != 2) || (atoi(strSplit[7].c_str()) != 0 && atoi(strSplit[7].c_str()) != 1 && atoi(strSplit[7].c_str()) != 2) ) {
			return NULL;
		}
	}

	//parse the request to argumet list
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

	//release raw message memory
	free(message);
	return args;
}

//request process function
string process(char **args, Graph *map) {

	int argc = atoi(args[0]);
	string key = args[1];
	string result = "Unknown request!";

	//process "trip" type request
	if (key == "trip") {
		string type0 = args[2];
		tripType type;
		string fromVertex = args[3];
		string toVertex = args[4];
		string roadLabel = fromVertex + "_" + toVertex;
		string suffix = "_" + type0;
		string roadFullLabel = roadLabel + suffix;
		vector<string>* path;
		if (type0 == "fastest") {
			type = FASTEST;
		}
		if (type0 == "shortest") {
			type = SHORTEST;
		}

		//lock to access the buffer_map
		pthread_mutex_lock(&lock_map);
	
		if (map->containsRoad(roadFullLabel)) {
			path = map->getRoad(roadFullLabel);
			result = fromVertex;
			string v1 = fromVertex;
			string v2;
	
			for (int i = 0; i < path->size(); i++) {
				v2 = map->getV2(v1, (*path)[i]); 
				result = result + " -> " + v2;
				v1 = v2;
			}

		} else {
		
			if (map->containsVertex(fromVertex) && map->containsVertex(toVertex)) {
				bool findTrip = map->trip(fromVertex, toVertex, roadLabel, type);
				if (findTrip) {
					path = map->getRoad(roadFullLabel);
					result = fromVertex;
					string v1 = fromVertex;
					string v2;

					for (int i = 0; i < path->size(); i++) {
						v2 = map->getV2(v1, (*path)[i]); 
						result = result + " -> " + v2;
						v1 = v2;
					}
		
				} else {
					result = roadFullLabel + "not found!";
				}
			} else {
				result = "Unknown start/end vertex!";
			}
		}
	
		//unlock the mutex on the buffer_map
		pthread_mutex_unlock(&lock_map);
	}


	//process "add" type requests
	if (key == "add") {
	
		string roadLabel = args[2];
		string fromVertex = args[3];
		string toVertex = args[4];
		int dir0 = atoi(args[5]);
		dirType dir;
		if (dir0 == 0) {
			dir = BI_DIRECTIONAL;	
		} else if (dir == 1) {
			dir = V1_TO_V2;	
		} else {
			dir = V2_TO_V1;	
		}

		int speed = atoi(args[6]);
		int length = atoi(args[7]);
		int type0 = atoi(args[8]);
		eventType type;
		if (type0 == 0) {
			type = OPEN;	
		} else if (type == 1) {
			type = CLOSE;	
		} else {
			type = HAZARD;	
		}

		//lock to access the buffer_raw
		pthread_mutex_lock(&lock_map);

		if (!map->containsVertex(fromVertex)) {
			map->addVertex(fromVertex, DEFAULT_VERTEXTYPE, DEFAULT_X, DEFAULT_Y);
			//cout << "add fromVertex in the server map!" << endl;
		}

		if (!map->containsVertex(toVertex)) {
			map->addVertex(toVertex, DEFAULT_VERTEXTYPE, DEFAULT_X, DEFAULT_Y);
			//cout << "add toVertex in the server map!" << endl;
		}
		if (!map->containsEdge(fromVertex+"_"+toVertex)) {
			map->addEdge(fromVertex+"_"+toVertex, fromVertex, toVertex, dir, speed, length, type);
			result = "road " + fromVertex + "_" + toVertex + "(" + args[5] + " " + args[6] + " " + args[7] + " " + args[8] + ") has been added in the server map!";
		} else {
			result = "road " + fromVertex + "_" + toVertex + "(" + args[5] + " " + args[6] + " " + args[7] + " " + args[8] + ") already exists in the server map!";
		}
	
		//unlock the mutex on the buffer_map
		pthread_mutex_unlock(&lock_map);

	}

	//free argument list memory
	int num_args = atoi(args[0])+1;
	for (int i = 0; i < num_args; i++) {
		free(args[i]);
	}
	free(args);

	return result;

}

//producer thread
void *producer(void *parameters) {
	//get parameters
	struct producer_parms *args = (struct producer_parms *) parameters;
	queue<struct request_raw> *bufIn = args->bufIn;
	queue<struct request_parsed> *bufOut = args->bufOut;

	struct request_raw rq_raw;
	struct request_parsed rq_parsed;

	while(1) {
		sem_wait(&occupied_raw);

		//lock to access the buffer_raw
		pthread_mutex_lock(&lock_raw);

		rq_raw = bufIn->front();
		bufIn->pop();

		//unlock the mutex on the buffer_raw
		pthread_mutex_unlock(&lock_raw);


		sem_post(&available_raw);
	

		//parse and valid the raw message
		int connfd = rq_raw.connfd;
		char* message = rq_raw.message;

		char** args = parse(message);
		if (args == NULL) {
			//reply to client
			char msg[] = "It is not a valid request!";
			send(connfd, msg, sizeof(msg), 0);
			close(connfd);
		} else {
			//send the valid message into the buffer_parsed
			rq_parsed.connfd = connfd;
			rq_parsed.args = args;
	
			sem_wait(&available_parsed);
	
			//lock to access the buffer_parsed
			pthread_mutex_lock(&lock_parsed);
	
			bufOut->push(rq_parsed);
	
			//unlock the mutex on the buffer_parsed
			pthread_mutex_unlock(&lock_parsed);
	
			sem_post(&occupied_parsed);
		}

	}

	return NULL;
}

//consumer thread
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
		
		string result = process(args, map);
	
		send(connfd, result.c_str(), result.size()+1, 0);
		close(connfd);
		}

	return NULL;
}



int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	//variable declaration
	int listenfd;
	int connfd;
	int port;
	//char *haddrp;
	socklen_t clientlen;
	struct sockaddr_in clientaddr;
	//struct hostent *hp;
	//pthread_t tid;
	struct request_raw request_raw;
	struct request_raw test;

	port = atoi(argv[1]);
	int P = atoi(argv[2]);
	int C = atoi(argv[3]);
	pthread_t list_p[P];
	pthread_t list_c[C];
	clientlen = sizeof(clientaddr);

	listenfd = open_listenfd(port);

	//create two buffers
	queue<struct request_raw> buf_raw;	
	queue<struct request_parsed> buf_parsed;	

	Graph m1;
	m1.retrieve("./resources/test1.txt");
	
	//semaphore initialization
	sem_init (&available_raw, 0, BUFFER_SIZE_RAW);
	sem_init (&occupied_raw, 0, 0);
	sem_init (&available_parsed, 0, BUFFER_SIZE_PARSED);
	sem_init (&occupied_parsed, 0, 0);

	//thread parameter initialization
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

	while (1) {

		//waiting for requests
		connfd = accept(listenfd, (SA *) &clientaddr, &clientlen);

		//receive client requests
		char *message = new char[MAXLINE];
		recv(connfd, message, MAXLINE, 0);

		request_raw.connfd = connfd;
		request_raw.message = message;

		//push raw requests in the buf_raw 
		sem_wait(&available_raw);

		//lock
		pthread_mutex_lock(&lock_raw);

		buf_raw.push(request_raw);
		
		//unlock
		pthread_mutex_unlock(&lock_raw);

		sem_post(&occupied_raw);
	}
	return 0;
}


