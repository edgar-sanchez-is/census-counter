//++++++++++++++++++++++++++++++++++++++++++
// Author: Luis D. Chaparro
// Course: CSCE_3600
// Due Date: 04/12/2016
// DescriptionL: Implementation of scokets
//++++++++++++++++++++++++++++++++++++++++++
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){
	// Counters
	int i;
	int x = -1;
	int s = -1;
	srand(time(NULL));
	int percent = 1;			// Get at least 20% returns
	char tickets[15][6] = {0};	// Array to hold tickets
	for(i = 0; i < 15; i++)		// Initialize array to 0
		strcpy(tickets[i], "0");
	// Main loop
	for(i = 0; i < 15; i++){	
		int sockfd, portno, n;
		struct sockaddr_in serv_addr;
		struct hostent *server;
	
		char buffer[256];
		if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
		}
		portno = atoi(argv[2]);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) 
			error("ERROR opening socket");
		server = gethostbyname(argv[1]);
		if (server == NULL) {
			fprintf(stderr,"ERROR, no such host\n");
			exit(0);
		}
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, 
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);
		serv_addr.sin_port = htons(portno);
		if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
			error("ERROR connecting");
		
		// Return random value
		if( ((rand() % 10000 + 100 ) % 7 == 0 && percent < 4) || (percent != 4 && x >= 10) ){
			//printf("Percent: %d\n", percent);
			percent++;
			s++;
			char msg[24] = "[CLIENT 2]: RETURN ";
			strcat(msg, tickets[s]);
			printf("[CLIENT]: RETURN %s\n", msg+19);
			n = write(sockfd, msg, sizeof(msg));
			strcpy(tickets[s], "0");
			if(n < 0) 
				error("ERROR writing to socket");
			bzero(buffer,256);
			n = read(sockfd,buffer,255);
		
			if(n < 0) 
				error("ERROR reading from socket");
		
			printf("[SERVER]: %s\n", buffer);
			printf("[CLIENT]: Ticket %s returned\n", msg+19);	
		}
		// Buy Ticket
		else{
			printf("[CLIENT]: BUY\n");
			n = write(sockfd, "[CLIENT 2]: BUY", 15);
			if(n < 0) 
				error("ERROR writing to socket");
				
			bzero(buffer,256);
			n = read(sockfd,buffer,255);
		
			if(n < 0) 
				error("ERROR reading from socket");
		
			printf("[SERVER]: %s\n", buffer);
			if(!strcmp(buffer, "Database full")){
				printf("[CLIENT]: Buy failed\n");
				continue;
			}
			else if(!strcmp(buffer, "Invalid ticket provided")){
				printf("Ticket not exist\n");
				continue;
			}
			else
				strcpy(tickets[++x],buffer);
		}
	
		close(sockfd);
	}
	printf("[CLIENT]: Database Table:\n");
	for(i = 0; i < 15; i++){
		printf("[%d] %s\n", i, tickets[i]);
	}
    return 0;
}

