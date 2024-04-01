#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define PORT 12345
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

int main() {
    int sockfd, epoll_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("UDP server is running and waiting for messages...\n");

    // Create an epoll instance
    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event event, events[MAX_EVENTS];
    event.events = EPOLLIN;  // Read events
    event.data.fd = sockfd;

    // Add the socket to the epoll instance
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == sockfd) {
                // Socket is ready for reading
                ssize_t bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                                                   (struct sockaddr *)&client_addr, &client_addr_len);
                if (bytes_received == -1) {
                    perror("recvfrom");
                    continue;
                }

                buffer[bytes_received] = '\0';  // Add null terminator to make it a valid C string

                printf("Message received from client: %s\n", buffer);
            }
            // Here you can add code to handle other events if needed
        }
    }

    // Close the socket and epoll instance (this part of the code is unreachable)
    close(sockfd);
    close(epoll_fd);

    return 0;
}

