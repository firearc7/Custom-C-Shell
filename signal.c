#include "signal.h"

void ping(char** args){
    if (args[1]==NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "ping: missing operand\n");
        fprintf(stderr, WHITE);
        return;
    }
    if (args[2]==NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "ping: missing operand\n");
        fprintf(stderr, WHITE);
        return;
    }
    if (args[3]!=NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "ping: too many arguments\n");
        fprintf(stderr, WHITE);
        return;
    }
    char* pid_str= args[1];
    int pid= atoi(pid_str);
    if (pid==0){
        fprintf(stderr, RED);
        fprintf(stderr, "ping: invalid argument\n");
        fprintf(stderr, WHITE);
        return;
    }
    char* signal_str= args[2];
    int signal= atoi(signal_str);
    signal%= 32;
    if (signal==0){
        fprintf(stderr, RED);
        fprintf(stderr, "ping: invalid argument\n");
        fprintf(stderr, WHITE);
        return;
    }

    if (kill(pid, 0)==-1){
        if (errno==ESRCH){
            fprintf(stderr, RED);
            fprintf(stderr, "ping: process does not exist\n");
            fprintf(stderr, WHITE);
            return;
        }
        else{
            fprintf(stderr, RED);
            perror("kill() error");
            fprintf(stderr, WHITE);
            return;
        }
    }

    int signal_sent;
    if ((signal_sent= kill(pid, signal))==-1){
        fprintf(stderr, RED);
        perror("kill() error");
        fprintf(stderr, WHITE);
        return;
    }
    else{
        printf("Signal %d sent to process %d\n", signal, pid);
    }
}

void fg_cmd(char** args){
    if (args[1]==NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "fg: missing operand\n");
        fprintf(stderr, WHITE);
        return;
    }
    if (args[2]!=NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "fg: too many arguments\n");
        fprintf(stderr, WHITE);
        return;
    }
    char* pid_str= args[1];
    int pid= atoi(pid_str);
    if (pid==0){
        fprintf(stderr, RED);
        fprintf(stderr, "fg: invalid argument\n");
        fprintf(stderr, WHITE);
        return;
    }

    if (kill(pid, 0)==-1){
        if (errno==ESRCH){
            fprintf(stderr, RED);
            fprintf(stderr, "fg: process does not exist\n");
            fprintf(stderr, WHITE);
            return;
        }
        else{
            fprintf(stderr, RED);
            perror("kill() error");
            fprintf(stderr, WHITE);
            return;
        }
    }

    int signal_sent;
    if ((signal_sent= kill(pid, SIGCONT))==-1){
        fprintf(stderr, RED);
        perror("kill() error");
        fprintf(stderr, WHITE);
        return;
    }
    else{
        time_t start_time= time(NULL);
        setpgid(pid, 0);
        signal(SIGTTOU, SIG_IGN);
        tcsetpgrp(STDIN_FILENO, pid);
        int status;
        waitpid(pid, &status, WUNTRACED);
        tcsetpgrp(STDIN_FILENO, getpgrp());
        time_t end_time= time(NULL);
        process_time= difftime(end_time, start_time);
        strcpy(process_name, "fg");
    }
}

void bg_cmd(char** args){
    if (args[1]==NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "bg: missing operand\n");
        fprintf(stderr, WHITE);
        return;
    }
    if (args[2]!=NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "bg: too many arguments\n");
        fprintf(stderr, WHITE);
        return;
    }
    char* pid_str= args[1];
    int pid= atoi(pid_str);
    if (pid==0){
        fprintf(stderr, RED);
        fprintf(stderr, "bg: invalid argument\n");
        fprintf(stderr, WHITE);
        return;
    }

    if (kill(pid, 0)==-1){
        if (errno==ESRCH){
            fprintf(stderr, RED);
            fprintf(stderr, "bg: process does not exist\n");
            fprintf(stderr, WHITE);
            return;
        }
        else{
            fprintf(stderr, RED);
            perror("kill() error");
            fprintf(stderr, WHITE);
            return;
        }
    }

    int signal_sent;
    if ((signal_sent= kill(pid, SIGCONT))==-1){
        fprintf(stderr, RED);
        perror("kill() error");
        fprintf(stderr, WHITE);
        return;
    }
    else{
        printf("[%d]\n", pid);
    }
}