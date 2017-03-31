#include "executable.h"
#include "sfish.h"

void sfish_exec(char** args){
	typedef struct stat stats;
	char* fullPath;
	stats* fileStats = malloc(sizeof(stats));
	char* tmp = getenv("PATH");
	char* searchPaths = strdup(tmp);
	if(strchr(args[0], '/') != NULL){
		fullPath = getenv("PWD");
		strcat(fullPath, "/");
		strcat(fullPath, args[0]);
		if(stat(fullPath, fileStats) == -1){
			printf("stat error: %s\n", strerror(errno));
			return;
		}
	}
	else{
		//printf("searchPaths: %s\n", searchPaths);
		char** searchPathsArray = getArgs(searchPaths);
		int pos = 0;
		char* currPath = searchPathsArray[pos];
		while(currPath != NULL){
			fullPath = strdup(currPath);
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
			return;
		}
	}

	int pid;
	int child_status;
	if((pid = fork()) < 0){
		printf("pwd fork error: %s\n", strerror(errno));
		exit(EXIT_SUCCESS);
	}

	if(pid == 0){
		execv(fullPath, args);
	}
	else
		wait(&child_status);
	free(searchPaths);

}
