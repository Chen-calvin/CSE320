#include "builtin.h"

void help(){
	printf("help\n");
	printf("exit\n");
	printf("cd [dir]\n");
	printf("pwd\n");
}

void pwd(){
	int pid;
	int child_status;
	if((pid = fork()) < 0){
		printf("pwd fork error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(pid == 0){
		char path[1024];
		if(getcwd(path, 1024) != NULL){
			printf("%s\n", path);
			exit(EXIT_SUCCESS);
		}
		else{
			printf("getcwd error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	else{
		wait(&child_status);
	}
}

void cd(char* flags){
	char* lastDir = "-";
	char* homeDir = " ";
	//char* prevDir = "..";
	//char* currDir = ".";

	if(strcmp(flags, lastDir) == 0){
		char* pwd = getenv("OLDPWD");
		chdir(pwd);
		setenv("OLDPWD", getenv("PWD"), 1);
		setenv("PWD", pwd, 1);
	}
		else if(strcmp(flags, homeDir) == 0){
			char* oldPwd = getenv("PWD");
			char* pwd = getenv("HOME");
			chdir(pwd);
			setenv("PWD", pwd, 1);
			setenv("OLDPWD", oldPwd, 1);
		}
			else{
				char* tmp = getenv("PWD");
				char path[1024];
				strcpy(path, tmp);
				strcat(path, "/");
				strcat(path, flags);
				if(chdir(path) == -1){
					printf("chdir error: %s\n", strerror(errno));
					return;
				}
				setenv("OLDPWD", tmp, 1);
				char newPWD[1024];
				if(getcwd(newPWD, 1024) == NULL){
					printf("getcwd error: %s\n", strerror(errno));
					return;
				}
				setenv("PWD", newPWD, 1);
			}
}

