// adapted fron "TCP/IP Sockets in C: Practical Guide for Programmers" by Donahoo, Calvert
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVERPORT 4950    // the port users will be connecting to
#define BROADCAST_ADDR "255.255.255.255"

// simple error handler
void Die(char *mess) { perror(mess); exit(1); }

int main(int argc, char *argv[]) {
    int sock, received;
    struct sockaddr_in connector; // connector's address information
    
    // make sure they passed the correct args
    if (argc != 2) {
        fprintf(stderr,"usage: broad message\n");
        exit(1);
    }
    
    // attempt to create the socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        Die("Socket creation failed");
    }
    
    // acquire permission to broadcast packets
    int broadcast = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) != 0) {
        Die("Couldn't set broadcast permissions on socket");
    }
    
    // configure the socket protocols, addresses, port, etc
    memset(&connector, 0, sizeof(connector));  // Clear struct
    connector.sin_family = AF_INET;     // Protocol: Internet/IP
    connector.sin_port = htons(SERVERPORT); // Port Number (hardcoded to 4950)
    connector.sin_addr.s_addr = inet_addr(BROADCAST_ADDR); //Broadcast to everyone
    
    // here goes...
    struct sockaddr *dest = (struct sockaddr*)&connector; //cast socket address
    if ((received=sendto(sock, argv[1], strlen(argv[1]), 0, dest, sizeof *dest)) == -1) {
        Die("Failed to broadcast message using sendto");
    }
    
    printf("broadcasted %d bytes to %s\n", received, inet_ntoa(connector.sin_addr));
    
    close(sock);
    
    return 0;
}
