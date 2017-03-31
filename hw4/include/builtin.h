#ifndef BUILTIN_H
#define BUILTIN_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <libgen.h>
#define PATH_MAX 1024

void help();
void pwd();
void cd(char* cmd);

#endif
