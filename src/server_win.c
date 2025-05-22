// server_win.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ctype.h>   // for isdigit()
#include <time.h>    // for timestamps

#pragma comment(lib, "ws2_32.lib")  // Link with Winsock library

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Log with timestamp
void log_to_file(FILE* logFile, const char* message) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec,
        message);
    fflush(logFile);
}

// Validate input format "I|player|button"
int validate_input(const char* input) {
    if (input[0] != 'I' || input[1] != '|') return 0;

    int pipe_count = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '|') pipe_count++;
    }
    if (pipe_count != 2) return 0;
    if (!isdigit((unsigned char)input[2])) return 0;
    if (input[3] != '|') return 0;
    if (input[4] == '\0') return 0;
    if (input[5] != '\0' && input[5] != '\n') return 0;

    return 1;
}

void func(SOCKET connfd, FILE* logFile) {
    char buff[MAX];
    int n;

    for (;;) {
        memset(buff, 0, sizeof(buff));
        int bytesReceived = recv(connfd, buff, sizeof(buff) - 1, 0);
        if (bytesReceived == 0) {
            printf("Client disconnected gracefully.\n");
            break;
        }
        if (bytesReceived < 0) {
            printf("Recv error or connection lost.\n");
            break;
        }
        buff[bytesReceived] = '\0';  // Null-terminate string

        printf("From client (raw): %s\t", buff);

        if (!validate_input(buff)) {
            printf("Invalid input format received.\n");
            const char* errMsg = "ERR|Invalid input format";
            send(connfd, errMsg, strlen(errMsg), 0);
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

        // Send structured ACK message
        char ackMsg[MAX];
        snprintf(ackMsg, sizeof(ackMsg), "ACK|%s|%s|%s", inputType, player, button);
        send(connfd, ackMsg, strlen(ackMsg), 0);

        if (strncmp(inputType, "exit", 4) == 0) {
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

    FILE* logFile = fopen("server.log", "a");
    if (!logFile) {
        printf("Failed to open log file.\n");
        return 1;
    }

    WSAStartup(MAKEWORD(2, 2), &wsa);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed...\n");
        fprintf(logFile, "Socket creation failed.\n");
        fclose(logFile);
        return 1;
    }
    printf("Socket successfully created..\n");
    log_to_file(logFile, "Socket successfully created.");

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("Socket bind failed...\n");
        log_to_file(logFile, "Socket bind failed.");
        closesocket(sockfd);
        WSACleanup();
        fclose(logFile);
        return 1;
    }
    printf("Socket successfully binded..\n");
    log_to_file(logFile, "Socket successfully binded.");

    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        log_to_file(logFile, "Listen failed.");
        closesocket(sockfd);
        WSACleanup();
        fclose(logFile);
        return 1;
    }
    printf("Server listening..\n");
    log_to_file(logFile, "Server listening.");

    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd == INVALID_SOCKET) {
        printf("Server accept failed...\n");
        log_to_file(logFile, "Server accept failed.");
        closesocket(sockfd);
        WSACleanup();
        fclose(logFile);
        return 1;
    }
    printf("Server accepted the client...\n");
    log_to_file(logFile, "Client connected.");

    func(connfd, logFile);

    closesocket(connfd);
    closesocket(sockfd);
    WSACleanup();
    log_to_file(logFile, "Server shut down.");
    fclose(logFile);
    return 0;
}
