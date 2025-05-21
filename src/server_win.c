// server_win.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ctype.h>  // for isdigit()

#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Validate input format "I|player|button"
int validate_input(const char* input) {
    // Check starts with 'I|'
    if (input[0] != 'I' || input[1] != '|') {
        return 0;
    }

    // Count '|' characters
    int pipe_count = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '|') pipe_count++;
    }
    if (pipe_count != 2) return 0;

    // Check player is digit and button is single char after second '|'
    // Expected format: I|<player>|<button>
    if (!isdigit((unsigned char)input[2])) return 0;
    if (input[3] != '|') return 0;

    // Button char must exist and be followed only by '\0' or '\n'
    if (input[4] == '\0') return 0;
    if (input[5] != '\0' && input[5] != '\n') return 0;

    return 1;
}

void func(SOCKET connfd) {
    char buff[MAX];
    int n;

    for (;;) {
        memset(buff, 0, sizeof(buff));
        int bytesReceived = recv(connfd, buff, sizeof(buff) - 1, 0);
        if (bytesReceived <= 0) {
            printf("Connection closed or error.\n");
            break;
        }
        buff[bytesReceived] = '\0';  // Null-terminate string

        printf("From client (raw): %s\t", buff);

        if (!validate_input(buff)) {
            printf("Invalid input format received.\n");
            const char* invalidMsg = "Invalid input format";
            send(connfd, invalidMsg, strlen(invalidMsg), 0);
            continue;
        }

        // Parse the input format: I|player|button
        char *token;
        char inputType[10], player[10], button[10];

        token = strtok(buff, "|");
        if (token != NULL) strcpy(inputType, token);
        else strcpy(inputType, "Unknown");

        token = strtok(NULL, "|");
        if (token != NULL) strcpy(player, token);
        else strcpy(player, "?");

        token = strtok(NULL, "|");
        if (token != NULL) strcpy(button, token);
        else strcpy(button, "?");

        printf("Parsed Input: %s, Player %s, %s button\n", inputType, player, button);

        // Clear buffer for response
        memset(buff, 0, sizeof(buff));
        n = 0;
        printf("Enter response: ");
        while ((buff[n++] = getchar()) != '\n' && n < MAX);

        send(connfd, buff, strlen(buff), 0);

        if (strncmp(buff, "exit", 4) == 0) {
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
