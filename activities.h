#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include "helper.h"

typedef struct process{
    int pid;
    char name[BUFSIZE];
} process;

typedef struct process_list{
    process* processes;
    int size;
} process_list;

bool does_process_exist(int pid);
bool is_process_running(int pid);
int compare_process(const void* a, const void* b);
void add_process(int pid, char* name);
void delete_process(int pid);
void print_processes();

#endif