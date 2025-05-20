// server_win.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void func(SOCKET connfd) {
    char buff[MAX];
    int n;

    for (;;) {
        memset(buff, 0, sizeof(buff));
        recv(connfd, buff, sizeof(buff), 0);
        printf("From client: %s\tTo client: ", buff);

        memset(buff, 0, sizeof(buff));
        n = 0;
        while ((buff[n++] = getchar()) != '\n');

        send(connfd, buff, sizeof(buff), 0);

        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

int main() {
    WSADATA wsa;
    SOCKET sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    int len;

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed...\n");
        return 1;
    }
    printf("Socket successfully created..\n");

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("Socket bind failed...\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    printf("Socket successfully binded..\n");

    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    printf("Server listening..\n");

    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd == INVALID_SOCKET) {
        printf("Server accept failed...\n");
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }
    printf("Server accepted the client...\n");

    func(connfd);

    closesocket(connfd);
    closesocket(sockfd);
    WSACleanup();
    return 0;
}
