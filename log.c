#include "log.h"

void add_to_log(char* command, char* home){
    if (in("log ", command) || strcmp(strip(command), "log")==0){
        return;
    }
    char* filename= (char*) malloc(BUFSIZE*sizeof(char));
    if (filename==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, RED);
        return;
    }
    strcpy(filename, home);
    strcat(filename, "/log.txt");
    FILE* log= fopen(filename, "r");
    if (log==NULL){
        fprintf(stderr, RED);
        perror("fopen() error");
        fprintf(stderr, WHITE);
        free(filename);
        return;
    }

    char* commands[15];
    char buffer[BUFSIZE];
    int lines_count = 0;

    while (lines_count < 15 && fgets(buffer, sizeof(buffer), log)!=NULL){
        buffer[strcspn(buffer, "\n")]= '\0';
        commands[lines_count]= strdup(buffer);
        if (commands[lines_count]==NULL){
            fprintf(stderr, RED);
            perror("strdup() error");
            fprintf(stderr, WHITE);
            for (int i= 0; i<lines_count; i++){
                free(commands[i]);
            }
            free(filename);
            fclose(log);
            return;
        }
        lines_count++;
    }
    fclose(log);

    if (lines_count>0 && strcmp(strip(commands[lines_count-1]), strip(command))==0){
        for (int i= 0; i<lines_count; i++){
            free(commands[i]);
        }
        free(filename);
        return;
    }

    if (lines_count==15){
        free(commands[0]);
        for (int i= 1; i<15; i++){
            commands[i-1]= commands[i];
        }
        lines_count--;
    }

    commands[lines_count]= strdup(command);
    if (commands[lines_count]==NULL){
        fprintf(stderr, RED);
        perror("strdup() error");
        fprintf(stderr, WHITE);
        for (int i = 0; i<lines_count; i++){
            free(commands[i]);
        }
        free(filename);
        return;
    }
    lines_count++;

    log= fopen(filename, "w");
    if (log==NULL){
        fprintf(stderr, RED);
        perror("fopen() error");
        fprintf(stderr, WHITE);
        for (int i=0; i<lines_count; i++){
            free(commands[i]);
        }
        free(filename);
        return;
    }

    for (int i=0; i<lines_count; i++){
        fprintf(log, "%s\n", commands[i]);
        free(commands[i]);
    }
    fclose(log);
    free(filename);

}

void purge_log(char* home){
    char* filename= (char*) malloc(BUFSIZE* sizeof(char));
    if (filename==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return;
    }
    strcpy(filename, home);
    strcat(filename, "/log.txt");
    FILE* log= fopen(filename, "w");
    if (log==NULL){
        fprintf(stderr, RED);
        perror("fopen() error");
        fprintf(stderr, WHITE);
        free(filename);
        return;
    }
    fclose(log);
    free(filename);
}

void execute_log(int index, char* home, char* pwd, char* cwd){
    char* filename= (char*) malloc(BUFSIZE * sizeof(char));
    if (filename==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return;
    }
    strcpy(filename, home);
    strcat(filename, "/log.txt");
    FILE* log= fopen(filename, "r");
    if (log==NULL){
        fprintf(stderr, RED);
        perror("fopen() error");
        fprintf(stderr, WHITE);
        free(filename);
        return;
    }
    char* commands[15];
    char buffer[BUFSIZE];
    int lines_count= 0;

    while (lines_count<15 && fgets(buffer, sizeof(buffer), log)!=NULL){
        buffer[strcspn(buffer, "\n")]= '\0';
        commands[lines_count]= strdup(buffer);
        if (commands[lines_count]==NULL) {
            fprintf(stderr, RED);
            perror("strdup() error");
            fprintf(stderr, WHITE);
            for (int i= 0; i<lines_count; i++) {
                free(commands[i]);
            }
            free(filename);
            fclose(log);
            return;
        }
        lines_count++;
    }
    fclose(log);
    if (index>lines_count){
        fprintf(stderr, RED);
        perror("Invalid index");
        fprintf(stderr, WHITE);
        for (int i=0; i<lines_count; i++){
            free(commands[i]);
        }
        free(filename);
        return;
    }
    split_command(commands[lines_count-index], home, cwd, pwd);
    for (int i=0; i<lines_count; i++){
        free(commands[i]);
    }
    free(filename);
}


void log_cmd(char** args, char* home, char* pwd, char* cwd){
    char* filename= (char*) malloc(BUFSIZE* sizeof(char));
    if (filename==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return;
    }
    strcpy(filename, home);
    strcat(filename, "/log.txt");

    if (args[1]==NULL){
        FILE* log= fopen(filename, "r");
        if (log==NULL){
            fprintf(stderr, RED);
            perror("fopen() error");
            fprintf(stderr, WHITE);
            free(filename);
            return;
        }
        char command[BUFSIZE];
        while (fgets(command, BUFSIZE, log)!=NULL){
            printf("%s", command);
        }
        fclose(log);
    }
    else if (strcmp(args[1], "purge")==0){
        if (args[2]!=NULL){
            fprintf(stderr, RED);
            perror("Invalid argument");
            fprintf(stderr, WHITE);
            free(filename);
            return;
        }
        purge_log(home);
    }
    else if (strcmp(args[1], "execute")==0){
        if (args[2]==NULL){
            fprintf(stderr, RED);
            perror("Invalid index");
            fprintf(stderr, WHITE);
            free(filename);
            return;
        }
        int index= atoi(args[2]);
        if (args[3]!=NULL){
            fprintf(stderr, RED);
            perror("Invalid argument");
            fprintf(stderr, WHITE);
            free(filename);
            return;
        }
        if (index<=0){
            fprintf(stderr, RED);
            perror("Invalid index");
            fprintf(stderr, WHITE);
            free(filename);
            return;
        }
        if (index>15){
            fprintf(stderr, RED);
            perror("Index out of bounds");
            fprintf(stderr, WHITE);
            free(filename);
            return;
        }
        char* home_copy= strdup(home);
        execute_log(index, home, pwd, cwd);
        strcpy(home, home_copy);
        free(home_copy);
    }
    else{
        fprintf(stderr, RED);
        perror("Invalid argument");
        fprintf(stderr, WHITE);
    }
    free(filename);
}