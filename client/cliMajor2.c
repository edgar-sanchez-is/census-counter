// =====================================================================
// Names:           Alex, Edgar, Luis, Russell
// Course:          CSCE 3600
// Due Date:        May 4, 2016
// Title:           Major Assignment 2 - Client
// Version:         1.0
// Description:     This program will consist of a client/server and
//                  client/client model using Linux sockets for a
//                  "census" system using the UNT Apollo server.
// Format:          ./client <rem_ipaddr> <svr_port> <cli1_port> <cli2_port> <cpu_%>
// =====================================================================

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// Function prototypes
void error(const char *);

int main(int argc, char *argv[]) {
	// Checks if user provided necessary arguments
	if (argc < 3) {
		fprintf(stderr, "Usage: ./client <rem_ipaddr> <svr_port> <cli1_port> <cli2_port> <cpu_%%>\n");
		exit(0);
	}

	// Declares variables
	int sockfd, portno;
	ssize_t n;
	struct sockaddr_in serv_addr;
	struct hostent* server;
	char buffer[256];

	// Initializes socket
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy(server->h_addr, (char*) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR connecting");
	}

	// Receives clientID from server
	char clientID[2];
	bzero(clientID, 2);
	n = read(sockfd, clientID, 6);
	if (n < 0) {
		error("ERROR reading from socket");
	}

	// Loops while user input is a positive integer
	while (1) {
		// Prompts user for census data and stores it in a buffer
		printf("[Client %s] Enter census data: ", clientID);
		bzero(buffer, 256);
		fgets(buffer, 255, stdin);

		// Writes census data to server
		n = write(sockfd, buffer, strlen(buffer));
		if (n < 0) {
			error("ERROR writing to socket");
		}

		// Reads server reply and stores it in a buffer
		bzero(buffer, 256);
		n = read(sockfd, buffer, 6);
		if (n < 0) {
			error("ERROR reading from socket");
		}

		// Processes server reply
		if (strncmp(buffer, "CPU", 3) == 0) {
			// TODO: Monitor CPU usage
			printf("==> [MAX CPU] : CPU Utilization X | CPU Threshold 8%%\n");
			return EXIT_FAILURE;
		}
		else if (strncmp(buffer, "CLOSE", 6) == 0) {
			// Listens for CLOSE confirmation from server then exits
			printf("Disconnecting...\n");
			return EXIT_SUCCESS;
		}
		else {
			// Prints total census count from server
			printf("==> [TOTAL] : %s\n", buffer);
		}
	}
	close(sockfd);
	return EXIT_SUCCESS;
}

void error(const char *msg)
{
	perror(msg);
	exit(0);
}