#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket, fdmax, i, j;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    fd_set master_set, read_fds;
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);

    FD_ZERO(&master_set);
    FD_ZERO(&read_fds);
    FD_SET(server_fd, &master_set);
    fdmax = server_fd;

    printf("Select Server listening on port %d...\n", PORT);

    while (1) {
        read_fds = master_set;
        select(fdmax + 1, &read_fds, NULL, NULL, NULL);

        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == server_fd) {
                    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > fdmax) fdmax = new_socket;
                    printf("New client connected.\n");
                } else {
                    int valread = recv(i, buffer, BUFFER_SIZE, 0);
                    if (valread <= 0) { 
                        close(i);
                        FD_CLR(i, &master_set);
                        printf("Client disconnected.\n");
                    } else {
                        buffer[valread] = '\0';
                        for (j = 0; j <= fdmax; j++) {
                            if (FD_ISSET(j, &master_set) && j != server_fd && j != i) {
                                send(j, buffer, valread, 0); 
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}