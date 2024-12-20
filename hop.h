#ifndef HOP_H
#define HOP_H

#include "helper.h"

void switch_directory(char* path);
void hop(char* cwd, char* pwd, char* home, char* path);
void hop_multiple(char** args, char* cwd, char* pwd, char* home);

#endif