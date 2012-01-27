// adapted fron "TCP/IP Sockets in C: Practical Guide for Programmers" by Donahoo, Calvert
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define PORT "4950" //string because getaddrinfo expects char*
#define BUFFERSIZE 255 // arbitrary limit, to prevent overflow

void Die(char *msg) { perror(msg); exit(1); }

// updates file contents based on UDP packet contents
void updateFile(char* msg) {
    FILE *file = fopen("synch.txt","w+"); 
    fprintf(file,"%s\n", msg);
    fclose(file);
}

int main(int argc, char *argv[]) {
    // criteria needed for a client to connect to this server
    struct addrinfo criteria;               // Criteria for potential clients
    memset(&criteria, 0, sizeof(criteria)); // Clear address memory
    criteria.ai_family = AF_INET;           // Only IPv4 for simplicity
    criteria.ai_flags = AI_PASSIVE;         // Accepts connections from anyone
    criteria.ai_socktype = SOCK_DGRAM;      // Only allow UDP
    criteria.ai_protocol = IPPROTO_UDP;     // Only allow UDP (cont.)
    
    struct addrinfo *myaddress; // List of server addresses
    int rtnVal = getaddrinfo(NULL, PORT, &criteria, &myaddress);
    if (rtnVal != 0) {
        Die("getaddrinfo() failed: " + rtnVal);
    }
    
    // create socket for client connections
    int sock;
    if ((sock = socket(myaddress->ai_family, myaddress->ai_socktype,
                       myaddress->ai_protocol)) < 0)
        Die("Can't create socket");
    
    // listen on local port for client connections 
    if (bind(sock, myaddress->ai_addr, myaddress->ai_addrlen) < 0)
        Die("Can't bind to port");
    
    printf("Listening on port %s\n", PORT);
    
    // Free memory used for address list allocated by getaddrinfo()
    freeaddrinfo(myaddress);
    
    while (1) { //infinite loop (terminates on SIGINT)
        struct sockaddr_storage clntAddr; // client address 
        // length of client address struct (it's easier to store it now)
        socklen_t clntAddrLen = sizeof(clntAddr);
        
        // create empty buffer to hold message
        char buffer[BUFFERSIZE];
        memset(&buffer, '\0', sizeof buffer);
        
        ssize_t numBytesRcvd;
        // Note: this blocks until client message reaches socket
        if ((numBytesRcvd = recvfrom(sock, buffer, BUFFERSIZE, 0,
               (struct sockaddr *) &clntAddr, &clntAddrLen)) < 0)
            Die("Receive message failed");
        
        printf("%s says: %s \n", inet_ntoa(((struct sockaddr_in *)&clntAddr)->sin_addr), buffer);
        
        updateFile(buffer);
    }
}
