#include "hop.h"

void switch_directory(char* path){
    int x= chdir(path);
    printf("%s\n", getcwd(NULL, 0));
    if (x==0){
        char cwd[BUFSIZE];
        if (getcwd(cwd, sizeof(cwd))==NULL){
            fprintf(stderr, RED);
            perror("getcwd() error");
            fprintf(stderr, WHITE);
            return;
        }
    }
    else{
        fprintf(stderr, RED);
        perror("chdir() error");
        fprintf(stderr, WHITE);
        return;
    }
}

void hop(char* cwd, char* pwd, char* home, char* path){
    if (strncmp(path, "-", 1)==0){
        if (pwd==NULL){
            fprintf(stderr, RED);
            perror("NULL pwd");
            fprintf(stderr, WHITE);
            return;
        }
        if (strlen(path)>1){
            fprintf(stderr, RED);
            perror("Invalid path");
            fprintf(stderr, WHITE);
        }
        switch_directory(pwd);
        return;
    }
    if (strncmp(path, "~", 1)==0){
        switch_directory(home);
        if (strlen(path)>2){
            path= path+2;
        }
        else{
            return;
        }
    }
    switch_directory(path);
}

void hop_multiple(char** args, char* cwd, char* pwd, char* home){
    int i= 1;
    if (args[1]==NULL){
        hop(cwd, pwd, home, home);
        return;
    }
    while (args[i]!=NULL){
        hop(cwd, pwd, home, args[i]);
        strcpy(pwd, cwd);
        strcpy(cwd, getcwd(NULL, 0));
        i++;
    }
}