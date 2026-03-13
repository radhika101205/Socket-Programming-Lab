#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8888
#define BUFFER_SIZE 1024

void *receive_messages(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[read_size] = '\0';
        printf("%s", buffer);
    }
    printf("\nServer disconnected.\n");
    exit(0);
}

int main() {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE];
    pthread_t recv_thread;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        return 1;
    }

    printf("Connected to server! Type your messages below:\n");
    pthread_create(&recv_thread, NULL, receive_messages, (void*)&sock);

    while (fgets(message, BUFFER_SIZE, stdin) != NULL) {
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("Send failed");
            break;
        }
    }

    close(sock);
    return 0;
}