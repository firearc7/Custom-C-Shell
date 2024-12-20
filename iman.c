#include "iman.h"

void iman(char** args){
    if (args==NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "iman: missing operand\n");
        fprintf(stderr, WHITE);
        return;
    }
    if (args[0]==NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "iman: missing operand\n");
        fprintf(stderr, WHITE);
        return;
    }

    char command[BUFSIZE-95];
    strncpy(command, args[1], sizeof(command)-1);
    command[sizeof(command)-1] = '\0';

    struct addrinfo hints, *res;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family= AF_UNSPEC;
    hints.ai_socktype= SOCK_STREAM;

    if (getaddrinfo("man.he.net", "http", &hints, &res)!=0){
        fprintf(stderr, RED);
        fprintf(stderr, "iman: cannot resolve %s\n", command);
        fprintf(stderr, WHITE);
        return;
    }

    sockfd= socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (connect(sockfd, res->ai_addr, res->ai_addrlen)==-1){
        fprintf(stderr, RED);
        fprintf(stderr, "iman: cannot connect to %s\n", command);
        fprintf(stderr, WHITE);
        return;
    }

    freeaddrinfo(res);

    char request[BUFSIZE];
    snprintf(request, sizeof(request), "GET /?topic=%s&section=all HTTP/1.0\r\n""Host: man.he.net\r\n""Connection: close\r\n\r\n", command);
    if (send(sockfd, request, strlen(request), 0)==-1){
        fprintf(stderr, RED);
        fprintf(stderr, "iman: cannot send request\n");
        fprintf(stderr, WHITE);
        return;
    }

    char buffer[1024];
    int bytes_received;
    bool header_ended= false;

    while ((bytes_received= recv(sockfd, buffer, sizeof(buffer), 0))>0){
        buffer[bytes_received]= '\0';
        char* content= buffer;

        if (!header_ended){
            content= strstr(buffer, "<");
            if (content){
                header_ended= true;
            }
            else{
                continue;
            }
        }
        bool in_tag= false;
        for (char* c= content; *c!='\0'; c++){
            if (*c=='<'){
                in_tag= true;
            }
            else if (*c=='>'){
                in_tag= false;
                continue;
            }
            else if (!in_tag){
                putchar(*c);
            }
        }
    }
    if (bytes_received==-1){
        fprintf(stderr, RED);
        fprintf(stderr, "iman: cannot receive response\n");
        fprintf(stderr, WHITE);
        return;
    }
    close(sockfd);
}