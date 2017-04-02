#ifndef HANDLER_C
#define HANDLER_C
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

typedef void (*sighandler_t)(int);

void sfish_alarm(int time);
void sigalrm_handler(int sig);
void sigusr2_handler(int sig);
void sigchld_handler(int sig, siginfo_t *sip, void *notused);
void init_signal();

#endif
