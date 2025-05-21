//
// Created by g0mes on 20/05/2025.
//
// client_win.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void func(SOCKET sockfd) {
    char buff[MAX];
    int n;
    for (;;) {
        memset(buff, 0, sizeof(buff));
        printf("Enter input (format I|player|button) or 'exit' to quit: ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n' && n < MAX);

        // Remove trailing newline from input
        if (buff[n - 1] == '\n') {
            buff[n - 1] = '\0';
        }

        send(sockfd, buff, strlen(buff), 0);

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client Exit...\n");
            break;
        }

        memset(buff, 0, sizeof(buff));
        int bytesReceived = recv(sockfd, buff, sizeof(buff) - 1, 0);
        if (bytesReceived <= 0) {
            printf("Connection closed by server or error.\n");
            break;
        }
        buff[bytesReceived] = '\0';
        printf("From Server: %s\n", buff);
    }
}

int main() {
    WSADATA wsa;
    SOCKET sockfd;
    struct sockaddr_in servaddr;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed...\n");
        return 1;
    }
    printf("Socket successfully created..\n");

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    printf("Connected to the server..\n");

    func(sockfd);

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
