// =====================================================================
// Names:           Alex, Edgar, Luis, Russell
// Course:          CSCE 3600
// Due Date:        May 4, 2016
// Title:           Major Assignment 2 - Server
// Version:         1.0
// Description:     This program will consist of a client/server and
//                  client/client model using Linux sockets for a
//                  "census" system using the UNT Apollo server.
// Format:          ./server <port>
// =====================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>

// Function prototypes
void processRequest(int, int **, size_t);
int error(const char *);

int main(int argc, char *argv[]) {
	// Checks if user provided necessary arguments
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		return EXIT_FAILURE;
	}

	// Declares variables
	int sockfd, newsockfd, portno;
	pid_t pid;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	// Opens INET TCP socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	// Allow socket to be reused immediately
	int option = 1;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option));

	// Initializes socket structure
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	// Binds the host address using bind() function
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}

	// Listens for clients and waits for incoming connections
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	printf("Waiting for incoming connections...\n");

	// Creates base ticket struct then creates a shared memory ticket struct pointing to base
	int *totalCensus  = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	// Client connection loop (allows two connections)
	size_t clientCounter = 1;
	while (clientCounter <= 2) {
		// Waits until client connects then stores new file descriptor in newsockfd
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			error("ERROR on accept");
		}

		printf("Connection accepted:     Client %zu\n", clientCounter);

		// Forks and processes client requests in child
		pid = fork();
		if (pid < 0) {
			// Error
			error("ERROR on fork");
		}
		else if (pid == 0) {
			// Child Process
			close(sockfd);
			processRequest(newsockfd, &totalCensus, clientCounter);
			close(newsockfd);
			return EXIT_SUCCESS;
		}
		else {
			// Parent Process
			close(newsockfd);
			clientCounter++;
		}
	}

	// Waits for forked processes to terminate and gracefully exits
	wait(NULL);
	wait(NULL);
	close(sockfd);
	munmap(&totalCensus, sizeof(unsigned int));
	return EXIT_SUCCESS;
}

// Processes input from client until user disconnects
void processRequest(int sock, int **totalCensus, size_t clientID) {
	ssize_t n;                                  // Stores read/write return value
	char buffer[256];                           // Stores buffered input from client
	int clientCensus;                  // Stores census data from client

	// Sends clientID to client
	char clientIDBuffer[2];
	sprintf(clientIDBuffer, "%zu", clientID);
	n = write(sock, clientIDBuffer, 4);
	if (n < 0) {
		error("ERROR writing to socket");
	}
	printf("Handler assigned:        Client %zu\n", clientID);


	// Loops while client input is a positive integer
	while (1) {
		// Reads census data from client into buffer
		bzero(buffer, 256);
		n = read(sock, buffer, 255);
		if (n < 0) {
			error("ERROR reading from socket");
		}

		// Parses census data string into unsigned int
		clientCensus = (int) strtoul(buffer, NULL, 0);

		// Processes census data from client
		if (clientCensus > 0) {
			// Adds clientCensus to totalCensus then prints total
			**totalCensus += clientCensus;
			printf("Received from client %zu:  %u\n", clientID, clientCensus);
			printf("          Census total:  %u\n", **totalCensus);


			// Writes census total to client
			char text[sizeof(int)]; // TODO: Declare outside of loop
			sprintf(text, "%u", **totalCensus);
			n = write(sock, text, 6);
			if (n < 0) {
				error("ERROR writing to socket");
			}
		}
		else if (clientCensus == 0){
			// Breaks from while loop if client sends 0 or invalid data
			printf("Disconnected: Client %zu\n", clientID);
			n = write(sock, "CLOSE", 6);
			if (n < 0) {
				error("ERROR writing to socket");
			}
			break;
		}
		else {
			// Breaks from while loop if client sends 0 or invalid data
			printf("Disconnected due to CPU threshhold: Client %zu\n", clientID);
			break;
		}
	}
}

int error(const char *msg) {
	perror(msg);
	return EXIT_FAILURE;
}