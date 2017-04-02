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
	printf("Well that was easy..");
	fflush(stdout);
}

void sigchld_handler(int sig, siginfo_t *sip, void *notused){
	printf("Child with PID %d has died. It spent %f milliseconds utilizing the CPU.\n",sip->si_pid, (double)((sip->si_utime + sip->si_stime) * 1000));
	fflush(stdout);
}

void init_signal(){
	sigset_t mask, oldMask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGTSTP);
	sigprocmask(SIG_BLOCK, &mask, &oldMask);

	signal(SIGUSR2, sigusr2_handler);
	signal(SIGALRM, sigalrm_handler);

	struct sigaction action;

	action.sa_sigaction = &sigchld_handler; /* Note use of sigaction, not handler */
	sigfillset (&action.sa_mask);
	action.sa_flags = SA_SIGINFO; /* Note flag - otherwise NULL in function */

	sigaction (SIGCHLD, &action, NULL);
}
