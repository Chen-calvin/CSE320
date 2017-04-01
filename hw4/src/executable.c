#include "executable.h"
#include "sfish.h"

int sfish_exec(char** args){
	typedef struct stat stats;
	char fullPath[1024];
	stats* fileStats = malloc(sizeof(stats));
	if(fileStats == NULL){
		printf("malloc error: %s\n", strerror(errno));
		return -1;
	}
	char* tmp = getenv("PATH");
	char* searchPaths = strdup(tmp);
	if(strchr(args[0], '/') != NULL){
		tmp = getenv("PWD");
		strcpy(fullPath,tmp);
		strcat(fullPath, "/");
		strcat(fullPath, args[0]);
		if(stat(fullPath, fileStats) == -1){
			printf("stat error: %s\n", strerror(errno));
			return -1;
		}
	}
	else{
		//printf("searchPaths: %s\n", searchPaths);
		char** searchPathsArray = getArgs(searchPaths);
		int pos = 0;
		char* currPath = searchPathsArray[pos];
		while(currPath != NULL){
			strcpy(fullPath, currPath);
			strcat(fullPath, "/");
			strcat(fullPath, args[0]);
			// printf("currPath%d: %s\n", pos, currPath);
			// printf("fullPath%d: %s\n", pos, fullPath);
			if(stat(fullPath, fileStats) == 0){
				break;
			}
			pos++;
			currPath = searchPathsArray[pos];
		}
		free(searchPathsArray);
		if(currPath == NULL){
			printf("stat error: %s\n", strerror(errno));
			return -1;
		}
	}

	int pid;
	int child_status;
	if((pid = fork()) < 0){
		printf("pwd fork error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(pid == 0){
		if((execv(fullPath, args)) < 0){
			printf("execv error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	else{
		wait(&child_status);
	}
	free(searchPaths);
	return 1;
}
