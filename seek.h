#ifndef REVEAL_H
#define REVEAL_H

#include "helper.h"
#include "hop.h"

int flags2(char* arg);
void walk(char* path, char* target, int show_d, int show_f);
void seek_cmd(char** args, char* home, char* pwd, char* cwd);

#endif