#ifndef REDIRECTION_H
#define REDIRECTION_H
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

int checkRedirection(char* cmd);
char** separateArgs(char* cmd, char* delimiters);
void redirection(char* args);

void inputRedirection(char* program[], char* file);
void outputRedirection(char* program[], char* file);
void inputOutputRedirection(char* program[], char* inFile, char* outFile);
void singlePipe(char* program1[], char* program2[]);
void doublePipe(char* program1[], char* program2[], char* program3[]);

#endif
