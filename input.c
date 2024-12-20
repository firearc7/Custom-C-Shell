#include "input.h"

int bg_process_count= 0;
process_info bg_processes[BUFSIZE];

void get_username(char* username, size_t size){
    struct passwd* pw;
    uid_t uid;
    uid= geteuid();
    pw= getpwuid(uid);
    if (pw==NULL){
        fprintf(stderr, RED);
        perror("getpwuid() error");
        fprintf(stderr, WHITE);
        return;
    }
    if (snprintf(username, size, "%s", pw->pw_name)==-1){
        fprintf(stderr, RED);
        perror("strncpy() error");
        fprintf(stderr, WHITE);
        return;
    }
}

void display_prompt(char* home){
    char cwd[BUFSIZE];
    char hostname[BUFSIZE];
    char username[BUFSIZE];
    if ((getcwd(cwd, sizeof(cwd)))==NULL){
        fprintf(stderr, RED);
        perror("getcwd() error");
        fprintf(stderr, WHITE);
        return;
    }
    get_username(username, sizeof(username));
    if (gethostname(hostname, sizeof(hostname))==-1){
        fprintf(stderr, RED);
        perror("gethostname() error");
        fprintf(stderr, WHITE);
        return;
    }
    if (strncmp(cwd, home, strlen(home))==0){
        if (process_time>=2){
            int ptime= (int) process_time;
            printf("<%s@%s:~%s %s %ds> ", username, hostname, cwd+strlen(home), process_name, ptime);
            process_name[0]= '\0';
            process_time= 0;
        }
        else{
            printf("<%s@%s:~%s> ", username, hostname, cwd+strlen(home));
        }
    }
    else{
        if (process_time>=2){
            int ptime= (int) process_time;
            printf("<%s@%s:%s %s %ds> ", username, hostname, cwd, process_name, ptime);
        }
        else{
            printf("<%s@%s:%s> ", username, hostname, cwd);
        }
    }
}

void sigchild_handler(int sig){
    int saved_errno= errno;
    pid_t pid;
    int status;
    while ((pid= waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED))>0){
        if (WIFEXITED(status)){
            delete_process(pid);
            if (WEXITSTATUS(status)==0){
                printf("[%d] exited normally\n", pid);
            }
            else{
                printf("[%d] exited with status %d\n", pid, WEXITSTATUS(status));
            }
            for (int i=0; i<bg_process_count; i++){
                if (bg_processes[i].pid==pid){
                    for (int j= i; j<bg_process_count-1; j++){
                        bg_processes[j]= bg_processes[j+1];
                    }
                    bg_process_count--;
                    break;
                }
            }
            break;
        }
        else if (WIFSTOPPED(status)){
            printf("[%d] stopped\n", pid);
            bg_processes[bg_process_count].pid= pid;
            bg_process_count++;
            break;
        }
    }
    errno= saved_errno;
}

void execute_command(char* command, char* home, char* cwd, char* pwd, bool bg){
    char* command_copy= strdup(command);
    if (command_copy==NULL){
        fprintf(stderr, RED);
        perror("strdup() error");
        fprintf(stderr, WHITE);
        return;
    }
    command_copy= replace(command_copy, "<", " < ");
    command_copy= replace(command_copy, ">", " > ");
    command_copy= replace(command_copy, ">  >", ">>");
    command_copy= strip(command_copy);
    char* token= strtok(command_copy, " ");
    char** args= (char**) malloc(BUFSIZE* sizeof(char*));
    for (int i=0; i<BUFSIZE; i++){
        args[i]= (char*) malloc(BUFSIZE* sizeof(char));
    }
    int i= 0;
    char* ins= (char*) malloc(BUFSIZE* sizeof(char));
    if (ins==NULL){
        fprintf(stderr, RED);
        perror("malloc() error");
        fprintf(stderr, WHITE);
        return;
    }
    int input_fd= -1;
    int output_fd= -1;
    int saved_stdin= dup(STDIN_FILENO);
    int saved_stdout= dup(STDOUT_FILENO);
    while (token!=NULL){
        char* redir_pos;
        if ((redir_pos= strstr(token, "<"))!=NULL){
            if (redir_pos!=token){
                strncpy(args[i++], token, redir_pos-token);
                args[i-1][redir_pos-token]= '\0';
            }
            char* filename= redir_pos+1;
            if (*filename=='\0'){
                token= strtok(NULL, " ");
                filename= token;
            }
            filename= strip(filename);
            input_fd= open(filename, O_RDONLY);
            if (input_fd==-1){
                fprintf(stderr, RED);
                perror("open() error");
                fprintf(stderr, WHITE);
                return;
            }
        }
        else if ((redir_pos= strstr(token, ">>"))!=NULL){
            if (redir_pos!=token){
                strncpy(args[i++], token, redir_pos-token);
                args[i-1][redir_pos-token]= '\0';
            }
            char* filename= redir_pos+2;
            if (*filename=='\0'){
                token= strtok(NULL, " ");
                filename= token;
            }
            filename= strip(filename);
            output_fd= open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (output_fd==-1){
                fprintf(stderr, RED);
                perror("open() error");
                fprintf(stderr, WHITE);
                return;
            }
        }
        else if ((redir_pos= strstr(token, ">"))!=NULL){
            if (redir_pos!=token){
                strncpy(args[i++], token, redir_pos-token);
                args[i-1][redir_pos-token]= '\0';
            }
            char* filename= redir_pos+1;
            if (*filename=='\0'){
                token= strtok(NULL, " ");
                filename= token;
            }
            filename= strip(filename);
            output_fd= open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd==-1){
                fprintf(stderr, RED);
                perror("open() error");
                fprintf(stderr, WHITE);
                return;
            }
        }
        else{
            strcpy(args[i++], token);
        }
        token= strtok(NULL, " ");
    }
    args[i]= NULL;
    strcpy(ins, strip(args[0]));

    if (input_fd!=-1){
        if (dup2(input_fd, 0)==-1){
            fprintf(stderr, RED);
            perror("dup2() error");
            fprintf(stderr, WHITE);
            return;
        }
        close(input_fd);
    }
    if (output_fd!=-1){
        if (dup2(output_fd, 1)==-1){
            fprintf(stderr, RED);
            perror("dup2() error");
            fprintf(stderr, WHITE);
            return;
        }
        close(output_fd);
    }

    if (strcmp((ins), "hop")==0){
        hop_multiple(args, cwd, pwd, home);
    }
    else if (strcmp((ins), "reveal")==0){
        reveal(args, home, pwd, cwd);
    }
    else if (strcmp((ins), "log")==0){
        log_cmd(args, home, pwd, cwd);
    }
    else if (strcmp((ins), "proclore")==0){
        proclore(args, home, pwd, cwd);
    }
    else if (strcmp((ins), "seek")==0){
        seek_cmd(args, home, pwd, cwd);
    }
    else if (strcmp((ins), "activities")==0){
        print_processes();
    }
    else if (strcmp((ins), "ping")==0){
        ping(args);
    }
    else if (strcmp((ins), "fg")==0){
        fg_cmd(args);
    }
    else if (strcmp((ins), "bg")==0){
        bg_cmd(args);
    }
    else if (strcmp((ins), "neonate")==0){
        neonate(args);
    }
    else if (strcmp((ins), "iMan")==0){
        iman(args);
    }
    else if (strcmp((ins), "exit")==0 || strcmp((ins), "quit")==0){
        for (int j=0; j<BUFSIZE; j++){
            free(args[j]);
        }
        free(ins);
        free(command_copy);
        free(args);
        exit(0);
    }
    else{
        char* cmd_copy= strdup(command);
        char* command_to_execute= (char*) malloc(BUFSIZE*sizeof(char));
        bool if_found1= false;
        if (cmd_copy==NULL){
            fprintf(stderr, RED);
            perror("strdup() error");
            fprintf(stderr, WHITE);
            return;
        }
        cmd_copy= strip(cmd_copy);
        for (int i=0; i<BUFSIZE; i++){
            if (aliases[i].init!=NULL && strcmp(aliases[i].init, cmd_copy)==0){
                if_found1= true;
                strcpy(command_to_execute, aliases[i].fin);
                break;
            }
        }
        if (if_found1){
            split_command(command_to_execute, home, cwd, pwd);
        }
        bool if_found2= false;
        for (int i=0; i<BUFSIZE; i++){
            if (funct[i].init!=NULL && strcmp(funct[i].init, ins)==0){
                if_found2= true;
                strcpy(command_to_execute, funct[i].fin1);
                strcat(command_to_execute, " ");
                strcat(command_to_execute, args[1]);
                strcat(command_to_execute, ";");
                strcat(command_to_execute, funct[i].fin2);
                strcat(command_to_execute, " ");
                strcat(command_to_execute, args[1]);
                strcat(command_to_execute, " ");
                break;
            }
        }
        if (if_found2){
            split_command(command_to_execute, home, cwd, pwd);
        }
        
        if (!if_found1 && !if_found2){
            if (bg){
                struct sigaction sa;
                sa.sa_handler= sigchild_handler;
                sigemptyset(&sa.sa_mask);
                sa.sa_flags= SA_RESTART | SA_NOCLDSTOP;
                if (sigaction(SIGCHLD, &sa, NULL)==-1){
                    fprintf(stderr, RED);
                    perror("sigaction() error");
                    fprintf(stderr, WHITE);
                    return;
                }
                pid_t pid1= fork();
                if (pid1==0){
                    setpgrp();
                    setpgid(0, 0);
                    signal(SIGINT, SIG_DFL);
                    signal(SIGTSTP, SIG_DFL);
                    if (execvp(ins, args)==-1){
                        fprintf(stderr, RED);
                        perror("execvp() error");
                        fprintf(stderr, WHITE);
                        delete_process(getpid());
                        exit(1);
                    }
                }
                else if (pid1<0){
                    fprintf(stderr, RED);
                    perror("fork() error");
                    fprintf(stderr, WHITE);
                    exit(1);
                }
                else{
                    printf("[%d]\n", pid1);
                    add_process(pid1, strip(join(args)));
                    bg_processes[bg_process_count].pid= pid1;
                    bg_process_count++;
                    waitpid(pid1, NULL, WNOHANG);
                }
            }
            else{
                time_t t1, t2;
                time(&t1);
                pid_t pid= fork();
                if (pid<0){
                    fprintf(stderr, RED);
                    perror("fork() error");
                    fprintf(stderr, WHITE);
                    exit(1);
                }
                else if (pid==0){
                    signal(SIGINT, SIG_DFL);
                    signal(SIGTSTP, SIG_DFL);
                    if (execvp(ins, args)==-1){
                        fprintf(stderr, RED);
                        perror("execvp() error");
                        fprintf(stderr, WHITE);
                        delete_process(getpid());
                        exit(1);
                    }
                }
                else{
                    add_process(pid, strip(join(args)));
                    setpgid(pid, 0);
                    signal(SIGTTOU, SIG_IGN); 
                    tcsetpgrp(STDIN_FILENO, pid);
                    int status;
                    waitpid(pid, &status, WUNTRACED);
                    tcsetpgrp(STDIN_FILENO, getpgrp());
                    time(&t2);
                    process_time= difftime(t2, t1);
                    strcpy(process_name, ins);
                    if (!WIFSTOPPED(status)){
                        delete_process(pid);
                    }
                    else{
                        printf("[%d] stopped\n", pid);
                    }
                }
            }
        }
    }
    if (dup2(saved_stdin, STDIN_FILENO)==-1){
        fprintf(stderr, RED);
        perror("dup2() error");
        fprintf(stderr, WHITE);
        return;
    }
    if (dup2(saved_stdout, STDOUT_FILENO)==-1){
        fprintf(stderr, RED);
        perror("dup2() error");
        fprintf(stderr, WHITE);
        return;
    }
    close(saved_stdin);
    close(saved_stdout);
    for (int j=0; j<BUFSIZE; j++){
        free(args[j]);
    }
    free(ins);
    free(command_copy);
    free(args);
}

void pipe_command(char* input, char* home, char* cwd, char* pwd, bool bg){
    int saved_stdin= dup(STDIN_FILENO);
    int saved_stdout= dup(STDOUT_FILENO);
    int input_fd= -1;
    int output_fd= -1;
    char* input_copy= strdup(input);
    if (input_copy==NULL){
        fprintf(stderr, RED);
        perror("strdup() error");
        fprintf(stderr, WHITE);
        return;
    }
    bool valid= false;
    while (*input_copy!='\0'){
        if (*input_copy==' '){
            input_copy++;
        }
        else if (*input_copy!='|'){
            valid= true;
            input_copy++;
        }
        else{
            if (!valid){
                fprintf(stderr, RED);
                perror("Invalid pipe command");
                fprintf(stderr, WHITE);
                return;
            }
            valid= false;
            input_copy++;
        }
    }
    if (!valid){
        fprintf(stderr, RED);
        perror("Invalid pipe command");
        fprintf(stderr, WHITE);
        return;
    }
    input_copy= strdup(input);
    char* token= strtok(input_copy, "|");
    char** commands= (char**) malloc(BUFSIZE* sizeof(char*));
    for (int i=0; i<BUFSIZE; i++){
        commands[i]= (char*) malloc(BUFSIZE* sizeof(char));
    }
    int i= 0;
    while (token!=NULL){
        token= strip(token);
        strcpy(commands[i++], token);
        token= strtok(NULL, "|");
    }
    if (i==1){
        execute_command(commands[0], home, cwd, pwd, bg);
    }
    else{
        int pipefd[2];
        int input_fd= dup(STDIN_FILENO);
        int saved_stdout= dup(STDOUT_FILENO);
        int saved_stdin= dup(STDIN_FILENO);

        for (int j= 0; j<i; j++){
            if (j<i-1){
                if (pipe(pipefd)==-1){
                    fprintf(stderr, RED);
                    perror("pipe() error");
                    fprintf(stderr, WHITE);
                    return;
                }
            }
            if (dup2(input_fd, STDIN_FILENO)==-1){
                fprintf(stderr, RED);
                perror("dup2() error");
                fprintf(stderr, WHITE);
                return;
            }
            if (j==i-1){
                if (dup2(saved_stdout, STDOUT_FILENO)==-1){
                    fprintf(stderr, RED);
                    perror("dup2() error");
                    fprintf(stderr, WHITE);
                    return;
                }
            }
            else{
                if (dup2(pipefd[1], STDOUT_FILENO)==-1){
                    fprintf(stderr, RED);
                    perror("dup2() error");
                    fprintf(stderr, WHITE);
                    return;
                }
            }
            close(pipefd[1]);
            if (j==i-1){
                execute_command(commands[j], home, cwd, pwd, bg);
            }
            else{
                execute_command(commands[j], home, cwd, pwd, false);
            }
            close(input_fd);
            input_fd= pipefd[0];
        }
        if (dup2(saved_stdin, STDIN_FILENO)==-1){
            fprintf(stderr, RED);
            perror("dup2() error");
            fprintf(stderr, WHITE);
            return;
        }
        if (dup2(saved_stdout, STDOUT_FILENO)==-1){
            fprintf(stderr, RED);
            perror("dup2() error");
            fprintf(stderr, WHITE);
            return;
        }
        close(saved_stdin);
        close(saved_stdout);
    }
    for (int j=0; j<BUFSIZE; j++){
        free(commands[j]);
    }
    free(input_copy);
    return;
}

void format_command(char* input, char* home, char* cwd, char* pwd){
    char* mega_command_copy= strdup(input);
    if (mega_command_copy==NULL){
        fprintf(stderr, RED);
        perror("strdup() error");
        fprintf(stderr, WHITE);
        return;
    }
    bool bg= false;
    mega_command_copy= strip(mega_command_copy);
    if (mega_command_copy[strlen(mega_command_copy)-1]=='&'){
        bg= true;
        mega_command_copy[strlen(mega_command_copy)-1]= '\0';
    }
    char* token= strtok(mega_command_copy, "&");
    char** commands= (char**) malloc(BUFSIZE* sizeof(char*));
    for (int i=0; i<BUFSIZE; i++){
        commands[i]= (char*) malloc(BUFSIZE* sizeof(char));
    }
    int i= 0;
    while (token!=NULL){
        strcpy(commands[i++], token);
        token= strtok(NULL, "&");
    }
    for (int j=0; j<i; j++){
        if (j==i-1){
            if (bg){
                pipe_command(commands[j], home, cwd, pwd, true);
            }
            else{
                pipe_command(commands[j], home, cwd, pwd, false);
            }
        }
        else{
            pipe_command(commands[j], home, cwd, pwd, true);
        }
    }
    for (int j=0; j<BUFSIZE; j++){
        free(commands[j]);
    }
}

void split_command(char* input, char* home, char* cwd, char* pwd){
    replace_tabs_with_spaces(input);
    char* input_copy= strdup(input);
    if (input_copy==NULL){
        fprintf(stderr, RED);
        perror("strdup() error");
        fprintf(stderr, WHITE);
        return;
    }
    char* token= strtok(input_copy, ";");
    char** mega_command= (char**) malloc(BUFSIZE* sizeof(char*));
    for(int i=0; i<BUFSIZE; i++){
        mega_command[i]= (char*) malloc(BUFSIZE* sizeof(char));
    }   
    int i=0;
    while (token!=NULL){
        strcpy(mega_command[i++], token);
        token= strtok(NULL, ";");
    }
    for (int j=0; j<i; j++){
        format_command(mega_command[j], home, cwd, pwd);
    }
    free(input_copy);
    for(int i=0; i<BUFSIZE; i++){
        free(mega_command[i]);
    }
}