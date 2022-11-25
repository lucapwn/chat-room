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
#include <errno.h>
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

#define LENGTH      2048
#define MAX_CLIENTS    3

#define RESET  "\e[0m"
#define RED    "\e[1;31m"
#define GREEN  "\e[1;32m"
#define YELLOW "\e[1;33m"
#define BLUE   "\e[1;34m"
#define PINK   "\e[1;35m"
#define CYAN   "\e[1;36m"
#define GRAY   "\e[1;90m"
#define DATE   "\e[7;36m"
#define OPEN   "\e[0;92m"
#define EXIT   "\e[0;91m"

int user_id = 0;
unsigned int clients_n = 0;

struct tm *p;
time_t seconds;

typedef struct {
    struct sockaddr_in address;
    int user_id;
    char name[30];
    int client_socket;
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void clear(char *text, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (text[i] == '\n') {
            text[i] = '\0';
            return;
        }
    }
}

void date_and_time() {
    time(&seconds);
    p = localtime(&seconds);

    char buffer[20];
    memset(buffer, '\0', 20);

    snprintf(buffer, 20, "%02d/%02d/%04d %02d:%02d:%02d",
        p->tm_mday,
        p->tm_mon + 1,
        p->tm_year + 1900,
        p->tm_hour,
        p->tm_min,
        p->tm_sec
    );

    printf(DATE "%s" RESET " ", buffer);
}

void client_address(struct sockaddr_in addr) {
    printf("%d.%d.%d.%d",
        (addr.sin_addr.s_addr & 0xff),
        (addr.sin_addr.s_addr & 0xff00) >> 8,
        (addr.sin_addr.s_addr & 0xff0000) >> 16,
        (addr.sin_addr.s_addr & 0xff000000) >> 24
    );
}

void adds_client_to_queue(client_t *client) {
    pthread_mutex_lock(&clients_mutex);

    for (size_t i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i]) {
            clients[i] = client;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void removes_client_from_queue(int user_id) {
    pthread_mutex_lock(&clients_mutex);

    for (size_t i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if (clients[i]->user_id == user_id) {
                clients[i] = NULL;
                break;
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void send_message(char *text, int user_id) {
    pthread_mutex_lock(&clients_mutex);

    for (size_t i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if (clients[i]->user_id != user_id) {
                if (write(clients[i]->client_socket, text, strlen(text)) == -1) {
                    fprintf(stderr, "[" RED "!" RESET "] An error occurred while sending the message to the users.\n");
                    break;
                }
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void *processes_client_communication(void *client_object) {
    bool flag = false;
    char name[30], buffer[LENGTH];

    clients_n++;
    client_t *client = (client_t *) client_object;

    if (recv(client->client_socket, name, 30, 0) <= 0 || strlen(name) < 3 || strlen(name) >= 28) {
        printf("Didn't enter the name.\n");
        flag = true;
    }
    else {
        strncpy(client->name, name, 30);
        sprintf(buffer, OPEN "%s has joined\n" RESET, client->name);
        date_and_time();
        printf("%s", buffer);
        send_message(buffer, client->user_id);
    }

    memset(buffer, '\0', LENGTH);

    while (true) {
        if (flag) {
            break;
        }

        int receive = recv(client->client_socket, buffer, LENGTH, 0);

        if (receive > 0) {
            if (strlen(buffer) > 0) {
                send_message(buffer, client->user_id);
                clear(buffer, strlen(buffer));
                date_and_time();
                printf("%s\n", buffer);
            }
        }
        else if (!receive || !strcmp(buffer, "exit")) {
            sprintf(buffer, EXIT "%s has left\n" RESET, client->name);
            date_and_time();
            printf("%s", buffer);
            send_message(buffer, client->user_id);
            flag = true;
        }
        else {
            fprintf(stderr, "An error occurred while processing the communication.\n");
            flag = true;
        }

        memset(buffer, '\0', LENGTH);
    }

    close(client->client_socket);
    removes_client_from_queue(client->user_id);
    free(client);
    clients_n--;
    pthread_detach(pthread_self());

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "[" RED "!" RESET "] Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char ip_address[] = "127.0.0.1";
    int port = atoi(argv[1]);

    int server_socket = 0;
    int connection = 0;
    int option = 1;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    server_socket               = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_address);
    server_addr.sin_port        = htons(port);

    signal(SIGPIPE, SIG_IGN);

    if (setsockopt(server_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *) &option, sizeof(option)) == -1) {
        fprintf(stderr, "[" RED "!" RESET "] An error occurred while setting the socket options.\n");
        return EXIT_FAILURE;
    }

    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "[" RED "!" RESET "] An error occurred while linking a name to the socket.\n");
        return EXIT_FAILURE;
    }

    if (listen(server_socket, 10) == -1) {
        fprintf(stderr, "[" RED "!" RESET "] An error occurred while accepting connection requests from clients.\n");
        return EXIT_FAILURE;
    }

    pthread_t thread_id;

    printf(CYAN "┌─┐┬ ┬┌─┐┌┬┐  ┬─┐┌─┐┌─┐┌┬┐                  \n"
                "│  ├─┤├─┤ │   ├┬┘│ ││ ││││                  \n"
                "└─┘┴ ┴┴ ┴ ┴   ┴└─└─┘└─┘┴ ┴ " RESET "(server)\n"
                "https://github.com/lucapwn                  \n");

    printf("\nWaiting for clients...\n\n");

    while (true) {
        socklen_t client_l = sizeof(client_addr);
        connection = accept(server_socket, (struct sockaddr *) &client_addr, &client_l);

        if (clients_n >= MAX_CLIENTS) {
            printf("The maximum customer quantity has been reached.\nClient rejected: ");
            client_address(client_addr);
            printf(":%d\n", client_addr.sin_port);
            close(connection);
            continue;
        }

        client_t *client = (client_t *) malloc(sizeof(client_t));
        client->address = client_addr;
        client->client_socket = connection;
        client->user_id = user_id++;

        adds_client_to_queue(client);
        pthread_create(&thread_id, NULL, &processes_client_communication, (void *) client);
        sleep(1);
    }

    return EXIT_SUCCESS;
}