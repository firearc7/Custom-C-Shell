#include "proclore.h"

void proclore(char** args, char* home, char* pwd, char* cwd){
    pid_t pid;
    if (args[1]!=NULL){
        pid= atoi(args[1]);
        if (args[2]!=NULL){
            fprintf(stderr, RED);
            perror("proclore() error");
            fprintf(stderr, WHITE);
            return;
        }
    }
    else{
        pid= getpid();
    }

    char proc_path[BUFSIZE-95];
    sprintf(proc_path, "/proc/%d", pid);

    char stat_path[BUFSIZE];
    sprintf(stat_path, "/proc/%d/stat", pid);

    FILE* stat_file= fopen(stat_path, "r");
    if (stat_file==NULL){
        fprintf(stderr, RED);
        perror("fopen() error");
        fprintf(stderr, WHITE);
        return;
    }

    int ppid, pgrp, session, tty_nr, tpgid;
    char comm[BUFSIZE], state;
    fscanf(stat_file, "%*d %s %c %d %d %d %d %d", comm, &state, &ppid, &pgrp, &session, &tty_nr, &tpgid);
    fclose(stat_file);

    char status[3];
    status[0]= state;
    status[1]= (tpgid==tcgetpgrp(STDIN_FILENO)) ? '+' : '\0';
    status[2]= '\0';

    char statm_path[BUFSIZE];
    sprintf(statm_path, "%s/statm", proc_path);
    FILE* statm_file= fopen(statm_path, "r");
    if (statm_file==NULL){
        fprintf(stderr, RED);
        perror("fopen() error");
        fprintf(stderr, WHITE);
        return;
    }

    unsigned long vsize;
    fscanf(statm_file, "%lu", &vsize);
    fclose(statm_file);

    char exe_path[BUFSIZE];
    sprintf(exe_path, "%s/exe", proc_path);
    char exe_realpath[BUFSIZE];
    ssize_t exe_realpath_len= readlink(exe_path, exe_realpath, sizeof(exe_realpath));
    if (exe_realpath_len==-1){
        fprintf(stderr, RED);
        perror("readlink() error");
        fprintf(stderr, WHITE);
        return;
    }
    else{
        exe_realpath[exe_realpath_len]= '\0';
    }
    printf("pid: %d\n", pid);
    printf("process status : %s\n", status);
    printf("Process Group : %d\n", pgrp);
    printf("Virtual memory : %lu\n", vsize);
    if (strncmp(exe_realpath, home, strlen(home))==0){
        printf("executable path : ~%s\n", exe_realpath+strlen(home));
    }
    else{
        printf("executable path : %s\n", exe_realpath);
    }
}