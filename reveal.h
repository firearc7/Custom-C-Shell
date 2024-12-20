#ifndef REVEAL_H
#define REVEAL_H

#include "helper.h"

int compare_strings(const void* a, const void* b);
int flags(char* arg);
void print_file_info(struct stat* file_stat, char* file_name);
void reveal(char** args, char* home, char* pwd, char* cwd);

#endif