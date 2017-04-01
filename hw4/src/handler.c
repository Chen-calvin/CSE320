#include "handler.h"
#include "debug.h"

int alarmTime;

void sfish_alarm(int time){
	if(time <= 0)
		printf("Invalid time\n");
	else{
		alarmTime = time;
		alarm(time);
	}
}

void sigalrm_handler(int sig){
	//write(STDOUT_FILENO, "\nYour ", 5);
	printf("\nYour %d second timer has finished!\n", alarmTime);
	//write(STDOUT_FILENO, " second timer had finished!", 64);
	fflush(stdout);
}

void sigusr2_handler(int sig){
	write(STDOUT_FILENO, "\nWell that was easy..", 20);
}

void sigchld_handler(int sig){
	//printf("Child with PID %d has died. It spent %d milliseconds utilizing the CPU.",,);
}

void init_signal(){
	sigset_t mask, oldMask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGTSTP);
	sigprocmask(SIG_BLOCK, &mask, &oldMask);

	signal(SIGUSR2, sigusr2_handler);
	signal(SIGALRM, sigalrm_handler);
}
