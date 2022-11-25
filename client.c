// MIT License

// Copyright (c) 2022 Lucas Araújo

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>

#define LENGTH 2048

#define RESET  "\e[0m"
#define RED    "\e[1;31m"
#define GREEN  "\e[1;32m"
#define YELLOW "\e[1;33m"
#define BLUE   "\e[1;34m"
#define PINK   "\e[1;35m"
#define CYAN   "\e[1;36m"
#define GRAY   "\e[1;90m"

char name[30];
bool flag = false;
int client_socket = 0;

void finalize() {
    flag = true;
}

void clear(char *text, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (text[i] == '\n') {
            text[i] = '\0';
            return;
        }
    }
}

void send_message() {
    srand(time(NULL));

    char color[8];
    char message[LENGTH];
    char buffer[LENGTH + 44]; // LENGTH + strlen(name) + strlen(color) + strlen(RESET)

    int n = rand() % 7;

    switch (n) {
        case 0:
            strcpy(color, RED);
            break;

        case 1:
            strcpy(color, GREEN);
            break;

        case 2:
            strcpy(color, YELLOW);
            break;

        case 3:
            strcpy(color, BLUE);
            break;

        case 4:
            strcpy(color, PINK);
            break;

        case 5:
            strcpy(color, CYAN);
            break;

        case 6:
            strcpy(color, GRAY);
            break;
    }

    while (true) {
        printf("> ");
        fflush(stdout);
        fgets(message, LENGTH, stdin);
        clear(message, LENGTH);

        if (strlen(message) > 0) {
            if (!strcmp(message, "exit")) {
                break;
            }
            else {
                snprintf(buffer, sizeof(buffer), "%s%s%s: %s\n", color, name, RESET, message);
                send(client_socket, buffer, strlen(buffer), 0);
            }
        }

        memset(message, '\0', LENGTH);
        memset(buffer, '\0', LENGTH + 44);
    }

    finalize();
}

void recv_message() {
    char message[LENGTH];

    while (true) {
        int receive = recv(client_socket, message, LENGTH, 0);

        if (receive > 0) {
            printf("%s> ", message);
            fflush(stdout);
        }
        else if (!receive) {
            break;
        }

        memset(message, '\0', LENGTH);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "[" RED "!" RESET "] Usage: %s <ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char ip_address[16];
    strncpy(ip_address, argv[1], 16);

    if (!strcmp(ip_address, "localhost")) {
        strncpy(ip_address, "127.0.0.1", 16);
    }

    int port = atoi(argv[2]);
    struct sockaddr_in server_addr;

    client_socket               = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_port        = htons(port);

    if (connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "[" RED "!" RESET "] An error occurred while connecting.\n");
        return EXIT_FAILURE;
    }

    printf(CYAN "┌─┐┬ ┬┌─┐┌┬┐  ┬─┐┌─┐┌─┐┌┬┐                  \n"
                "│  ├─┤├─┤ │   ├┬┘│ ││ ││││                  \n"
                "└─┘┴ ┴┴ ┴ ┴   ┴└─└─┘└─┘┴ ┴ " RESET "(client)\n"
                "https://github.com/lucapwn                  \n");

    signal(SIGINT, finalize);

    printf("\nWhat's your name? ");
    fgets(name, 30, stdin);
    clear(name, strlen(name));
    printf("\n");

    if (strlen(name) < 3 || strlen(name) > 28) {
        fprintf(stderr, "[" RED "!" RESET "] The name must be less than 28 and longer than 3 characters.\n");
        return EXIT_FAILURE;
    }

    send(client_socket, name, 28, 0);
    
    pthread_t send_message_thread;

    if (pthread_create(&send_message_thread, NULL, (void *) send_message, NULL)) {
        fprintf(stderr, "[" RED "!" RESET "] An error occurred when creating the thread.\n");
        return EXIT_FAILURE;
    }

    pthread_t recv_message_thread;

    if (pthread_create(&recv_message_thread, NULL, (void *) recv_message, NULL)) {
        fprintf(stderr, "[" RED "!" RESET "] An error occurred when creating the thread.\n");
        return EXIT_FAILURE;
    }

    while (true) {
        if (flag) {
            printf("\n[" GREEN "+" RESET "] Going out!\n");
            break;
        }
    }

    close(client_socket);

    return EXIT_SUCCESS;
}