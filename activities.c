#include "activities.h"

process_list* processes= NULL;
int process_count= 0;

bool does_process_exist(int pid){
    char proc_path[BUFSIZE-10];
    snprintf(proc_path, BUFSIZE, "/proc/%d", pid);

    char stat_path[BUFSIZE];
    snprintf(stat_path, BUFSIZE, "/proc/%d/stat", pid);

    FILE* stat_file= fopen(stat_path, "r");
    if (stat_file==NULL){
        delete_process(pid);
        return false;
    }
    else{
        fclose(stat_file);
        return true;
    }
}

bool is_process_running(int pid){
    char proc_path[BUFSIZE-10];
    snprintf(proc_path, BUFSIZE, "/proc/%d", pid);

    char stat_path[BUFSIZE];
    snprintf(stat_path, BUFSIZE, "/proc/%d/stat", pid);

    FILE* stat_file= fopen(stat_path, "r");
    if (stat_file==NULL){
        delete_process(pid);
        return false;
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
    snprintf(statm_path, BUFSIZE, "%s/statm", proc_path);
    FILE* statm_file= fopen(statm_path, "r");
    if (statm_file==NULL){
        delete_process(pid);
        return false;
    }

    unsigned long vsize;
    fscanf(statm_file, "%lu", &vsize);
    fclose(statm_file);

    char exe_path[BUFSIZE];
    snprintf(exe_path, BUFSIZE, "%s/exe", proc_path);
    char exe_realpath[BUFSIZE];
    ssize_t exe_realpath_len= readlink(exe_path, exe_realpath, sizeof(exe_realpath));
    if (exe_realpath_len==-1){
        return false;
    }
    else{
        exe_realpath[exe_realpath_len]= '\0';
    }
    if (state=='R' || state=='Z' || state=='S'){
        return true;
    }
    else{
        return false;
    }
}

int compare_process(const void* a, const void* b){
    process* p1= (process*) a;
    process* p2= (process*) b;
    return strcmp(p1->name, p2->name);
}

void add_process(int pid, char* name){
    if (processes==NULL){
        processes= (process_list*) malloc(sizeof(process_list));
        processes->processes= (process*) malloc(BUFSIZE* sizeof(process));
        processes->size= 0;
    }
    processes->processes[processes->size].pid= pid;
    strcpy(processes->processes[processes->size].name, name);
    processes->size++;
}

void delete_process(int pid){
    int saved_errno= errno;
    for (int i=0; i<processes->size; i++){
        if (processes->processes[i].pid==pid){
            for (int j=i; j<processes->size-1; j++){
                processes->processes[j]= processes->processes[j+1];
            }
            processes->size--;
            break;
        }
    }
    errno= saved_errno;
}

void print_processes(){
    if (processes==NULL){
        return;
    }
    for (int i=0; i<processes->size; i++){
        if (!does_process_exist(processes->processes[i].pid)){
            delete_process(processes->processes[i].pid);
        }
    }
    qsort(processes->processes, processes->size, sizeof(process), compare_process);
    for (int i=0; i<processes->size; i++){
        bool ran= false;
        if (is_process_running(processes->processes[i].pid)){
            ran= true;
        }
        printf("%d : %s - %s\n", processes->processes[i].pid, processes->processes[i].name, ran ? "running" : "stopped");
    }
}