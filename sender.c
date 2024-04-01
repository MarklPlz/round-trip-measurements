#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"  // IP address of the destination server
#define SERVER_PORT 12345       // Port number of the destination server
#define BUFFER_SIZE 1024        // Size of the buffer for sending/receiving data

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);
    char buffer[BUFFER_SIZE] = "Hello, UDP server!";

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    // Send a message to the server
    ssize_t bytes_sent = sendto(sockfd, buffer, BUFFER_SIZE, 0,
                                 (const struct sockaddr *)&server_addr, server_addr_len);
    if (bytes_sent == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    printf("Message sent to the server.\n");
    
    // Receive message from server
    ssize_t bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                                       (struct sockaddr *)&server_addr, &server_addr_len);
    if (bytes_received == -1) {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }
    printf("Message received from server.\n");

    // Close the socket
    close(sockfd);

    return 0;
}

