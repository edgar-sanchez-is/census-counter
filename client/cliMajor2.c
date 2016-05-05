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
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

// Function prototypes
void clientInit(int, char*, char*, int);
void listenerInit(int, int, int, int);
void remoteInit(int, char*, int, char*);
int cpuLoad();
void error(const char*);

int main(int argc, char *argv[]) {
	// Checks if user provided necessary arguments
	if (argc < 6) {
		fprintf(stderr, "Usage: ./client<#> <rem_ipaddr> <svr_port> <cli1_port> <cli2_port> <cpu_%%>\n");
		exit(0);
	}

	// Declares variables
	char *serverIP = "192.168.100.217";
	char *remoteIP = argv[1];
	int serverPort = atoi(argv[2]);
	int cpuLimit = atoi(argv[5]);

	ssize_t n = 0;

	// Listener variables
	int listenerPort = 0;
	int listenerSocket = 0;
	int listenerSocketNew = 0;

	// Remote variables
	int remotePort = 0;
	int remoteSocket = 0;

	if (strcmp(argv[0], "./client1") == 0) {
		listenerPort = atoi(argv[3]);
		remotePort  = atoi(argv[4]);
	}
	else if (strcmp(argv[0], "./client2") == 0) {
		listenerPort = atoi(argv[4]);
		remotePort  = atoi(argv[3]);
	}

	// Client variables
	char buffer[256];
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	char clientID[2];

	clientInit(serverSocket, clientID, serverIP, serverPort);

	pid_t pid = fork();
	if (pid < 0) {
		error("ERROR while forking");
	}
	else if (pid == 0) {
		// Child process
		listenerInit(listenerSocket, listenerPort, listenerSocketNew, serverSocket);
		printf("Enter census data: ");
		close(listenerSocket);
		close(listenerSocketNew);
		return EXIT_SUCCESS;
	}
	else {
		while (1) {
			// Prompts user for census data and stores it in a buffer
			printf("Enter census data: ");

			bzero(buffer, 256);
			fgets(buffer, 255, stdin);

			int cpuCurrent = cpuLoad();
			if (cpuCurrent > cpuLimit) {
				printf("  CPU: Utilization %i%% | Threshold %i%%\n", cpuCurrent, cpuLimit);
				remoteInit(remoteSocket, remoteIP, remotePort, buffer);

				n = write(serverSocket, "-1", 3);
				if (n < 0) {
					error("ERROR writing to socket");
				}

				break;
			}

			// Writes census data to server
			n = write(serverSocket, buffer, strlen(buffer));
			if (n < 0) {
				error("ERROR writing to socket");
			}
			printf("Sent to server:    %s", buffer);

			// Reads server reply (should be total) and stores it in a buffer
			bzero(buffer, 256);
			n = read(serverSocket, buffer, 255);
			if (n < 0) {
				error("ERROR reading from socket");
			}
			else if (strncmp(buffer, "CLOSE", 6) == 0) {
				// Listens for CLOSE confirmation from server then breaks from loop
				break;
			}
			else {
				// Prints total census count from server
				printf("  Census total:    %s\n", buffer);
			}
		}
	}

	// Loops while user input is a positive integer
	//wait(NULL);
	kill(pid, SIGKILL);
	close(serverSocket);
	close(listenerSocket);
	close(listenerSocketNew);
	close(remoteSocket);
	printf("Disconnected from server\n", clientID);
	return EXIT_SUCCESS;
}

void clientInit(int serverSocket, char* clientID, char *serverIP, int serverPort) {
	printf("SERVER IP:    %s\n", serverIP);
	printf("SERVER PORT:  %i\n", serverPort);

	struct sockaddr_in serv_addr;
	struct hostent* server = NULL;
	ssize_t n = 0;

	// Initializes socket
	if (serverSocket < 0)
		error("ERROR opening socket");
	server = gethostbyname(serverIP);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy(server->h_addr, (char*) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
	serv_addr.sin_port = htons(serverPort);
	if (connect(serverSocket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR connecting to server");
	}

	// Receives clientID from server
	bzero(clientID, 2);
	n = read(serverSocket, clientID, 6);
	if (n < 0) {
		error("ERROR reading from socket");
	}

	printf("CLIENT ID:    %s\n\n", clientID);
}

void listenerInit(int listenerSocket, int listenerPort, int listenerSocketNew, int serverSocket) {
	// Declares variables
	ssize_t n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char buffer[256];

	// Opens INET TCP socket
	listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenerSocket < 0) {
		error("ERROR opening socket");
	}

	// Initializes socket structure
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(listenerPort);

	// Binds the host address using bind() function
	if (bind(listenerSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}

	// Listens for clients and waits for incoming connections
	listen(listenerSocket,5);
	clilen = sizeof(cli_addr);

	listenerSocketNew = accept(listenerSocket, (struct sockaddr *) &cli_addr, &clilen);
	if (listenerSocketNew < 0) {
		error("ERROR on accept");
	}
	printf("Connection accepted:  Remote client\n");
	printf("Handler assigned:     Remote client\n");

	bzero(buffer, 256);
	n = read(listenerSocketNew, buffer, 255);
	if (n < 0) {
		error("ERROR writing to socket");
	}
	int clientCensus = (int) strtoul(buffer, NULL, 0);

	printf("Received %u from remote client\n", clientCensus);
	close(listenerSocketNew);
	printf("Disconnected:         Remote Client\n");

	// Writes census data to server
	n = write(serverSocket, buffer, strlen(buffer));
	if (n < 0) {
		error("ERROR writing to socket");
	}
	// Reads server reply (should be total) and stores it in a buffer
	bzero(buffer, 256);
	n = read(serverSocket, buffer, 255);
	if (n < 0) {
		error("ERROR reading from socket");
	}
	else {
		// Prints total census count from server
		printf("  Census total:    %s\n", buffer);
	}
}

void remoteInit(int remoteSocket, char* remoteIP, int remotePort, char* buffer) {
	struct sockaddr_in serv_addr;
	struct hostent* server = NULL;
	remoteSocket = socket(AF_INET, SOCK_STREAM, 0);
	int clientCensus = (int) strtoul(buffer, NULL, 0);
	ssize_t n;

	// Initializes socket
	if (remoteSocket < 0)
		error("ERROR opening socket");
	server = gethostbyname(remoteIP);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy(server->h_addr, (char*) &serv_addr.sin_addr.s_addr, (size_t) server->h_length);
	serv_addr.sin_port = htons(remotePort);
	if (connect(remoteSocket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		//error("ERROR connecting");
		fprintf(stderr, "Remote client is offline, census data not sent\n");
	}
	else {
		printf("MAX CPU : Sending %u to remote client\n", clientCensus);
		n = write(remoteSocket, buffer, strlen(buffer));
		if (n < 0) {
			error("ERROR reading from socket");
		}
	}
}

int cpuLoad() {
	long double a[4], b[4], loadavg;
	FILE *fp;

	fp = fopen("/proc/stat","r");
	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&a[0],&a[1],&a[2],&a[3]);
	fclose(fp);
	sleep(1);

	fp = fopen("/proc/stat","r");
	fscanf(fp,"%*s %Lf %Lf %Lf %Lf",&b[0],&b[1],&b[2],&b[3]);
	fclose(fp);

	loadavg = ((b[0]+b[1]+b[2]) - (a[0]+a[1]+a[2])) / ((b[0]+b[1]+b[2]+b[3]) - (a[0]+a[1]+a[2]+a[3]));

	return (int)(loadavg*100);
}

void error(const char* msg)
{
	perror(msg);
	exit(0);
}