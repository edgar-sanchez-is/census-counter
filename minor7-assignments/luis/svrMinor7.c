//++++++++++++++++++++++++++++++++++++++++++
// Author: Luis D. Chaparro
// Course: CSCE_3600
// Due Date: 04/12/2016
// DescriptionL: Implementation of scokets
//++++++++++++++++++++++++++++++++++++++++++
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/mman.h>
#include <netinet/in.h>
#define N 20
int tickets[N];
static int x = 0;
int* available;

void ticketService(int); /* function prototype */
void getTickets();
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
	available = mmap(NULL, sizeof *available, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	getTickets();
	
	int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
		error("setsockopt(SO_REUSEADDR) failed");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR on binding");
     listen(sockfd,2);
     clilen = sizeof(cli_addr);
     while(1){
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0){
             close(sockfd);
             ticketService(newsockfd);
             exit(0);
         }
         else{
			 if(x >= 20)
				 x = 0;
			 else 
				 x++;
			 close(newsockfd);
		 }
     } /* end of while */
     close(sockfd);
     return 0; /* we never get here */
}

/******** ticketService() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void ticketService (int sock){
	int n;
	char buffer[256];
	int ticket_Select = -1; 	// Holds selected ticket
	int noTickets = 0;			// If there are no tickets
	int i;						// counter
   
	bzero(buffer,256);
	n = read(sock,buffer,255);   
	if (n < 0) error("ERROR reading from socket");
	
	if((strncmp(buffer+12, "BUY", 3)) == 0 || (strncmp(buffer+12, "buy", 3)) == 0){
		if(available[x] == 1){
			ticket_Select = tickets[x];		// Selected ticket
			available[x] = 0;				// Ticket is not available anymore
		}else{								// Check one last time if there are any available tickets from returns
			for(i = 0; i < N; i++){
				if(available[i] == 1){
					ticket_Select = tickets[i];
					available[i] = 0;
					break;
				}else
					noTickets = 1;			// If no tickets available
			}			
		}
		
		// If no tickets
		if(noTickets == 1){
			printf("[SERVER X]: Database full\n");
			n = write(sock, "Database full", 13);
			if (n < 0) error("ERROR writing to socket");
		}else{
			printf("%s\n", buffer);
			printf("[SERVER X]: Client %.1s buy %d\n", buffer+8, ticket_Select);
			char num[5];
			sprintf(num, "%d", ticket_Select);
			n = write(sock, num, sizeof(num));
			if (n < 0) error("ERROR writing to socket");
		}
	}// If return 
	else if(!strncmp(buffer+12, "RETURN", 6) || !strncmp(buffer+12, "return", 6)){
		char num[5];
		int ticket_Return;
		strncpy(num, buffer+19, 5);
		sscanf(num, "%d", &ticket_Return);
		
		printf("%s\n", buffer);
		printf("[SERVER X]: Client %.1s cancel %d\n", buffer+8, ticket_Return);
		// Check for valid tickets
		int valid = 0;
		for(i = 0; i < N; i++){
			if(tickets[i] == ticket_Return){
				available[i] = 1;
				valid = 1;
				break;
			}
		}
		// If ticket is not valid
		if(!valid){
			// Invalid Ticket
			n = write(sock, "Invalid ticket provided", 23);
			if (n < 0) error("ERROR writing to socket");
		}			
		
		n = write(sock, buffer+12, sizeof(buffer+12));
		if (n < 0) error("ERROR writing to socket");
	}
	else{		
		n = write(sock, "Invalid input", 13);
		if (n < 0) error("ERROR writing to socket");
	}
}
void getTickets(){
	srand(time(NULL));
	// Generate random number from 10,000 to 99,999		
	int i, j;
	for(i = 0; i < 20; i++){
		tickets[i] = rand() % (99999+1-10000)+10000;
		available[i] = 1; // Tickets are available 
		if(i > 0){
			for(j = i-1; j >= 0; j--){
				if(tickets[i] == tickets[j]){
					tickets[i] = rand() % (99999+1-10000)+10000;
					i = 0;
				}				
			}
		}
	}
	//for(i = 0; i < 20; i++)
	//	printf("Tickets[%d]: %d\n", i+1, tickets[i]);
}