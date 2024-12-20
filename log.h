#ifndef LOG_H
#define LOG_H

#include "helper.h"
#include "input.h"

void add_to_log(char* command, char* home);
void purge_log(char* home);
void execute_log(int index, char* home, char* pwd, char* cwd);
void log_cmd(char** args, char* home, char* pwd, char* cwd);

#endif