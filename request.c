#include "client.h"

#define NUM_CLIENT 2


int main(int argc, char **argv) {

	int	port;
	char *host;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(0);
	}
	host = argv[1];
	port = atoi(argv[2]);

	//define client parameters
	pthread_t list_c[NUM_CLIENT];
	struct client_parms client_args;
	client_args.host = host;
	client_args.port = port;

	//begin to transfer data
	int i;
	for (i=0; i<NUM_CLIENT; i++) {
		if (pthread_create (&list_c[i], NULL, &client, &client_args)!=0) {
			fprintf(stderr, "client pthread_create error!\n");
		}
	}

	//join the producer and consumer threads
	for (i = 0; i < NUM_CLIENT; i++) {
		pthread_join (list_c[i], NULL);
	}


//	char hostname[20];
//	gethostname(hostname, sizeof(hostname));
//	fprintf(stderr, "hostname is %s\n", hostname);

	return 0;

}



