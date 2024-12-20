#ifndef SIGNAL_H
#define SIGNAL_H

#include "helper.h"
#include "activities.h"

extern char* process_name;
extern double process_time;

void ping(char** args);
void fg_cmd(char** args);
void bg_cmd(char** args);

#endif