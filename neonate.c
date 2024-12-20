#include "neonate.h"

int get_most_recent_pid(){
    FILE* file= fopen("/proc/loadavg", "r");
    if (file==NULL){
        fprintf(stderr, RED);
        perror("fopen() error");
        fprintf(stderr, WHITE);
        return -1;
    }
    char buffer[BUFSIZE];
    if (fgets(buffer, BUFSIZE, file)==NULL){
        fprintf(stderr, RED);
        perror("fgets() error");
        fprintf(stderr, WHITE);
        fclose(file);
        return -1;
    }
    fclose(file);
    char* token= strtok(buffer, " ");
    char* last_token= NULL;
    while (token!=NULL){
        last_token= token;
        token= strtok(NULL, " ");
    }
    if (last_token==NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "get_most_recent_pid: unexpected error\n");
        fprintf(stderr, WHITE);
        return -1;
    }
    return atoi(last_token);
}

void neonate(char** args){
    if (args[1]==NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "neonate: missing operand\n");
        fprintf(stderr, WHITE);
        return;
    }
    if (args[2]==NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "neonate: missing operand\n");
        fprintf(stderr, WHITE);
        return;
    }
    if (args[3]!=NULL){
        fprintf(stderr, RED);
        fprintf(stderr, "neonate: too many arguments\n");
        fprintf(stderr, WHITE);
        return;
    }
    if (strcmp(args[1], "-n")!=0){
        fprintf(stderr, RED);
        fprintf(stderr, "neonate: invalid option\n");
        fprintf(stderr, WHITE);
        return;
    }
    int n= atoi(args[2]);
    if (n<0){
        fprintf(stderr, RED);
        fprintf(stderr, "neonate: invalid argument\n");
        fprintf(stderr, WHITE);
        return;
    }
    int most_recent_pid= get_most_recent_pid();
    if (most_recent_pid==-1){
        return;
    }
    struct termios old_termios, new_termios;
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios= old_termios;
    new_termios.c_lflag&= ~(ICANON|ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    bool stop= false;
    int retval;
    time_t last_print_time= time(NULL);
    while (!stop){
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        struct timeval tv;
        tv.tv_sec= 0;
        tv.tv_usec= 0;
        retval= select(STDIN_FILENO+1, &readfds, NULL, NULL, &tv);
        if (retval==-1){
            if (errno==EINTR){
                continue;
            }
            fprintf(stderr, RED);
            perror("select() error");
            fprintf(stderr, WHITE);
            tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
            return;
        }
        else if (retval){
            char c;
            read(STDIN_FILENO, &c, 1);
            if (c=='x'){
                stop= true;
            }
        }
        time_t current_time= time(NULL);
        if (difftime(current_time, last_print_time)>=n){
            int new_most_recent_pid= get_most_recent_pid();
            if (new_most_recent_pid==-1){
                tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
                return;
            }
            if (new_most_recent_pid>most_recent_pid){
                most_recent_pid= new_most_recent_pid;
            }
            printf("\r%d\n", most_recent_pid);
            printf("\r");
            fflush(stdout);
            last_print_time= current_time;
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    return;
}