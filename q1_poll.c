#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#define PORT 8888
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    struct pollfd fds[MAX_CLIENTS + 1];
    int nfds = 1;
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;

    printf("Poll Server listening on port %d...\n", PORT);

    while (1) {
        poll(fds, nfds, -1);

        for (int i = 0; i < nfds; i++) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == server_fd) {
                    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    fds[nfds].fd = new_socket;
                    fds[nfds].events = POLLIN;
                    nfds++;
                    printf("New client connected. Total: %d\n", nfds - 1);
                } else {
                    int valread = recv(fds[i].fd, buffer, BUFFER_SIZE, 0);
                    if (valread <= 0) { 
                        close(fds[i].fd);
                        fds[i] = fds[nfds - 1];
                        nfds--;
                        i--;
                        printf("Client disconnected.\n");
                    } else {
                        buffer[valread] = '\0';
                        for (int j = 1; j < nfds; j++) {
                            if (fds[j].fd != fds[i].fd) {
                                send(fds[j].fd, buffer, valread, 0);
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}