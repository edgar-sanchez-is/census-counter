# Major Aassignment 2
###### Sockets &amp; Synchronization
###### Team: Alex, Edgar, Luis, Russell
--------

#### UNT VPN Client

Visit https://vpn.unt.edu to install the UNT VPN client on your computer. Use your UNT credentials to connect. 

You must be connected to VPN in order to access the Apollo server via SSH.

#### Apollo Server Login Credentials

| Group  | Apollo Server IP | Username |     Password     |
|:------:|:----------------:|:--------:|:----------------:|
|   14   |   129.120.60.29  |  group14 | csce3600group@14 |

#### Assigned Port Numbers

|     Virtual Machine    |   Port Number   |   Local IP Address   |
|:----------------------:|:---------------:|:--------------------:|
|   Server / Client #1   |      54217      |    192.168.100.217   |
|        Client #2       |      54152      |    192.168.100.152   |

#### Sample SSH Commands

###### Mac OS / Linux

    $ ssh -p [portnumber] group14@129.120.60.29
    
###### Windows

Windows does not have a native SSH client built in. 

Putty is the most popular SSH client for Windows.

Another option is to install Cmder and Cygwin to emulate a Linux Bash terminal on Windows.

Lastly, you may install the latest Windows 10 Insider Preview to use the new Bash on Ubuntu for Windows client.

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

- **major-assignment-2/**
    - **server/**
        - Makefile
        - svrMajor2.c
    - **client/**
        - Makefile
        - cliMajor2.c
    - **docs/**
        - groups-credentials.xlsx 
        - major2-instructions-with-samples.pdf
        - major2-presentation.pptx
    - **minor7-assignments/** (for reference)
    - README.md

#### Virtual Machine Directory Structure

###### Server / Client #1

- **~/major-assignment-2/**
  - **server/**
    - Makefile
    - svrMajor2.c
  - **client11/**
    - Makefile
    - cliMajor2.c

###### Client #2
- **~/major-assignment-2/**
  - **client2/**
    - Makefile
    - cliMajor2.c
