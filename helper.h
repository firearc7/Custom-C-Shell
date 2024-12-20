#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdbool.h>
#include <time.h>
#include <grp.h>
#include <termios.h>
#include <sys/socket.h>
#include <netdb.h>

#define RED "\033[0;31m"
#define WHITE "\033[0m"
#define GREEN "\033[0;32m"
#define BLUE "\033[0;34m"
#define BUFSIZE 4096

char* strip(char* str);
bool in(char* small_str, char* big_str);
void replace_tabs_with_spaces(char* str);
char* join(char** args);
char* replace(char* str, char* old, char* new);

#endif