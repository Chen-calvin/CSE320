#ifndef EXECUTABLE_H
#define EXECUTABLE_H
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

void sfish_exec(char** args);

#endif
