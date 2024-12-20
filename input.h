#ifndef INPUT_H
#define INPUT_H

#include "helper.h"
#include "hop.h"
#include "log.h"
#include "proclore.h"
#include "reveal.h"
#include "seek.h"
#include "activities.h"
#include "signal.h"
#include "neonate.h"
#include "iman.h"

extern char* process_name;
extern double process_time;

typedef struct {
    pid_t pid;
} process_info;

typedef struct {
    char* init;
    char* fin;
} alias;

typedef alias* alias_info;

typedef struct {
    char* init;
    char* fin1;
    char* fin2;
} func;

typedef func* func_info;

extern alias_info aliases;
extern func_info funct;
extern int process_count;
extern process_info bg_processes[BUFSIZE];

void get_username(char* username, size_t size);
void display_prompt(char* home);
void sigchild_handler(int sig);
void execute_command(char* command, char* home, char* cwd, char* pwd, bool bg);
void pipe_command(char* command, char* home, char* cwd, char* pwd, bool bg);
void format_command(char* input, char* home, char* cwd, char* pwd);
void split_command(char* input, char* home, char* cwd, char* pwd);

#endif