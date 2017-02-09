# Census Counter
###### Command line based census counter consisting of a client/server model using Linux network sockets
--------

#### Host Information

|     Host    |   Port Number   |   Local IP Address   |
|:----------------------:|:---------------:|:--------------------:|
|   Server / Client #1   |      [XXXXX]      |    [X.X.X.X]   |
|        Client #2       |      [XXXXX]      |    [X.X.X.X]   |

#### Program Execution

The following input input may be used to test the server / client program.

###### Server:

Place `server` directory on server.

    ./server <server port>
    
For example: `./server 51000`

###### Client 1

Place **client** directory on **client 1** and run the `client1` executable.

    ./client1 <remote client2 IP> <server port> <client1 port> <client2 port> <CPU threshold>
    
For example: `./client1 192.168.100.152 51000 52000 53000 13`

###### Client 2

Place **client** directory on **client 2** and run the `client2` executable.
    
    ./client2 <remote client2 IP> <server port> <client1 port> <client2 port> <CPU threshold>

For example: `./client2 192.168.100.217 51000 52000 53000 13`

![screenshot1](http://i.imgur.com/XdgDRR9.png)

#### Program Directory Stucture

- **census-counter/**
    - **server/**
        - Makefile
        - svrMajor2.c
    - **client/**
        - Makefile
        - cliMajor2.c
    - **docs/**
        - major2-instructions-with-samples.pdf
    - README.md

#### Host Directory Structure

###### Server / Client #1

- **~/census-counter/**
  - **server/**
    - Makefile
    - svrMajor2.c
  - **client11/**
    - Makefile
    - cliMajor2.c

###### Client #2
- **~/census-counter/**
  - **client2/**
    - Makefile
    - cliMajor2.c
