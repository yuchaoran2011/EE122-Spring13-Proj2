#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <math.h>
#include <limits.h>


#define DEFAULT_INTERVAL 6
#define DEFAULT_MSG "0"
#define NUM_PACKETS 1000

int R = DEFAULT_INTERVAL;
int allowed_sending = 0;



int poisson(float lamda) {
	int k = 0;
	float p = 1, L, u;
	L= exp(-lamda);
	while (1) {
		k++;
		u = (float) rand()/RAND_MAX;
		p = p * u;
		if (p < L) 
			break;
	}
	return k-1;
}



int main(int argc, char *argv[]) {

	int socketFD, sockfd2;
	int rate = DEFAULT_INTERVAL;
	char msg[141];
	
	struct addrinfo hints;
	struct addrinfo *servinfo;
	
	struct timespec sleepTime = {0, 100};

	FILE *input = fopen("MMSposter.jpg", "rb");


	fseek(input, 0L, SEEK_END);
 	int size = ftell(input);
 	fseek(input, 0L, SEEK_SET);

	
	//Get sockets
	socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket < 0){
		printf("Error: Could not get socket.");
		exit(1);
	}
	
	//Get connection info
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	if (getaddrinfo("10.10.66.90", "56790", &hints, &servinfo) != 0 || servinfo == NULL) {
	    printf("Error: Could not find router.");
	    exit(1);
	}

	

	int count = 1;
	int i;
	double timestamp;
	int tail = size % 128;


	printf("%d\n", tail);

	int a[32];
	while(fread(&a, 32*sizeof(int), 1, input) && !feof(input)) {
		sleepTime.tv_nsec = poisson((float)R)*1000000;
		nanosleep(&sleepTime, NULL);
		
		memcpy(msg,a,128);
		msg[128] = '1';   // flow number
		memcpy(msg+129, &count, 4);
		timestamp = clock()/(double)CLOCKS_PER_SEC;

		memcpy(msg+133, &timestamp, 8);
		

		if (sendto(socketFD, msg, 141, 0, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
			printf("Sending failed!\n");
			exit(1);
		}
		
		count += 1;
	}



	if (tail != 0) {
		fread(&a, tail, 1, input);
		sleepTime.tv_nsec = poisson((float)R)*1000000;
		nanosleep(&sleepTime, NULL);
		int temp = INT_MAX;
		
		memcpy(msg, &a, tail);
		msg[128] = '1';   // flow number
		memcpy(msg+129, &temp, 4);

		memcpy(msg+133, &tail,4);
		memcpy(msg+137, &count,4);

		printf("%d\n",*(int *)(msg+133));
		printf("%d\n",*(int *)(msg+137));

		if (sendto(socketFD, msg, 141, 0, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
			printf("Sending failed!\n");
			exit(1);
		}

		printf("%d\n", tail);
		
	}

	fclose(input);
	
	return 0;
}
	

