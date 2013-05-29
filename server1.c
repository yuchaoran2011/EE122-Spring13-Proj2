#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <limits.h>

int main(int argc, char *argv[]){

	int socketFD;
	char buf[141];
	int bytesReceived;
	
	struct addrinfo hints;
	struct addrinfo *servinfo;
	struct sockaddr_in clientinfo;
	int clientinfolen;

	FILE *output;
	char fname[] = "MMSposter_recv.jpg";
	int a[32], count, tail, pos;

	int packetsReceived1 = 0, packetsReceived2 = 0;

	
	//Get connection info
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;


	socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketFD < 0){
		printf("Error: Could not get socket.\n");
		exit(1);
	}
	
	if(getaddrinfo("10.10.66.90", "56890", &hints, &servinfo) != 0 || servinfo == NULL){
	    printf("Error: Could not get connection info with router.\n");
	    exit(1);
	}


	if(bind(socketFD, servinfo->ai_addr, servinfo->ai_addrlen) < 0){
		printf("Error: Could not bind socket.\n");
		exit(1);
	}

	output = fopen(fname, "wb");

	while (1) {

		clientinfolen = sizeof(struct sockaddr_in);
		if (recvfrom(socketFD, &buf, 141, 0, &clientinfo, &clientinfolen) == -1) {
			printf("Receiving failed!\n");
			exit(1);
		}
		
		memcpy(a, buf, sizeof(int)*32);
		memcpy(&count, buf+129, sizeof(int));

		if (count != INT_MAX) {
			fseek(output, 128*(count-1), SEEK_SET);
			fwrite(&a, sizeof(int), 32, output);
		} 
		else {
			tail = *(int *)(buf+133);
			pos = *(int *)(buf+137);
			printf("%d\n",tail);
			printf("%d\n",pos);
			fseek(output, 128*(pos-1), SEEK_SET);
			fwrite(&a, sizeof(char), tail, output);
			fclose(output);
		}

		packetsReceived1 += 1;
		printf("Flow: %c\n", buf[128]);
		printf("Count: %d\n", *(int *)(buf+129));
		printf("Time Stamp: %lf\n", *(double *)(buf+133));
		printf("%d\n", packetsReceived1);
		printf("\n");
	}	
}

	
	

