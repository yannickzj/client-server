#include "client.h"


int main(int argc, char **argv) {

	if (argc != 4) {
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(0);
	}
	char *host = argv[1];
	int port = atoi(argv[2]);
	int numClient = atoi(argv[3]);

	//define client parameters
	pthread_t list_c[numClient];
	struct client_parms client_args;
	client_args.host = host;
	client_args.port = port;

	//begin to transfer data
	int i;
	for (i=0; i<numClient; i++) {
		if (pthread_create (&list_c[i], NULL, &client, &client_args)!=0) {
			fprintf(stderr, "client pthread_create error!\n");
		}
	}

	//join the producer and consumer threads
	for (i = 0; i < numClient; i++) {
		pthread_join (list_c[i], NULL);
	}

	return 0;

}



