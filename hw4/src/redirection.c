#include "redirection.h"
#include "debug.h"
#include "executable.h"

int checkRedirection(char* cmd){
	int flag = 0;
	if(strchr(cmd, '>') || strchr(cmd, '<') || strchr(cmd, '|'))
		flag = 1;

	if(flag)
		return 1;
	return 0;
}

char** separateArgs(char* cmd, char* delimiters){
	int bufferSize = 64, pos = 0;
	char** tokens = malloc(bufferSize * sizeof(char*));
	if(tokens == NULL){
		printf("malloc error: %s\n", strerror(errno));
		return NULL;
	}
	char* token = strtok(cmd, delimiters);

	while(token != NULL){
		tokens[pos] = token;
		pos++;
		token = strtok(NULL, delimiters);
	}
	tokens[pos] = NULL;
	return tokens;
}

void redirection(char* args){
	char* inputFlag = strchr(args, '<');
	char* outputFlag = strchr(args, '>');
	int pipeCount = 0;
	int i =0;
	while(*(args + i) != '\0'){
		char tmp = *(args + i);
		if(tmp == '|')
			pipeCount++;
		i++;
	}

	// char** argsArray = separateArgs(args,);
	// if(argsArray == NULL){
	// 	perror("Malloc error");
	// 	return;
	// }

	if(inputFlag != NULL && outputFlag != NULL){
		//separate into program, inFile, and outFile and call inputOutputRedirection
		char** funcArgs = separateArgs(args, "<>\n");
		char* programArray = strdup(funcArgs[0]);
		char** program = separateArgs(programArray, " ");
		inputOutputRedirection(program, separateArgs(funcArgs[1], " ")[0], separateArgs(funcArgs[2], " ")[0]);
		free(programArray);
		free(funcArgs);
	}
	else if(inputFlag != NULL){
			//separate into program and inFile
			char** funcArgs = separateArgs(args, "<\n");
			char* programArray = strdup(funcArgs[0]);
			char** program = separateArgs(programArray, " ");
			inputRedirection(program, separateArgs(funcArgs[1], " ")[0]);
			free(programArray);
			free(funcArgs);
		}
		else if(outputFlag != NULL){
				//separate into program and outFile
				char** funcArgs = separateArgs(args, ">\n");
				char* programArray = strdup(funcArgs[0]);
				char** program = separateArgs(programArray, " ");
				outputRedirection(program, separateArgs(funcArgs[1], " ")[0]);
				free(programArray);
				free(funcArgs);
			}
			else if(pipeCount == 1){
					//separate into program1 and program2
					char** funcArgs = separateArgs(args, "|\n");
					char* programArray1 = strdup(funcArgs[0]);
					char* programArray2 = strdup(funcArgs[1]);
					char** program1 = separateArgs(programArray1, " ");
					char** program2 = separateArgs(programArray2, " ");
					singlePipe(program1, program2);
					free(program1);
					free(program2);
					free(programArray1);
					free(programArray2);
					free(funcArgs);
				}
				else if(pipeCount == 2){
					//separate into program1, program2, and program3
					char** funcArgs = separateArgs(args, "|");
					char* programArray1 = strdup(funcArgs[0]);
					char* programArray2 = strdup(funcArgs[1]);
					char* programArray3 = strdup(funcArgs[2]);
					char** program1 = separateArgs(programArray1, " ");
					char** program2 = separateArgs(programArray2, " ");
					char** program3 = separateArgs(programArray3, " \n");
					doublePipe(program1, program2, program3);
					free(program1);
					free(program2);
					free(program3);
					free(programArray1);
					free(programArray2);
					free(programArray3);
					free(funcArgs);
					}
					else
						fprintf(stderr,"Redirection error\n");
}

void inputRedirection(char* program[], char* file){
	int fd;
	int stdinFD = dup(STDIN_FILENO);

	if((fd = open(file, O_RDONLY)) < 0){
		printf("Open error: %s\n", strerror(errno));
		return;
	}

	if(dup2(fd, STDIN_FILENO) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}

	if(sfish_exec(program) < 0){
		printf("exec error: %s\n", strerror(errno));
		return;
	}

	if(close(fd) < 0){
		printf("Close error: %s\n", strerror(errno));
		return;
	}

	if(dup2(stdinFD, STDIN_FILENO) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}
}

void outputRedirection(char* program[], char* file){
	int fd;
	int stdoutFD;
	if((stdoutFD = dup(STDOUT_FILENO)) < 0){
		printf("dup error: %s\n", strerror(errno));
		return;
	}

	if((fd = open(file, O_WRONLY | O_TRUNC)) < 0){
		printf("Open error: %s\n", strerror(errno));
		return;
	}

	if(dup2(fd, STDOUT_FILENO) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}

	if(sfish_exec(program) < 0){
		printf("exec error: %s\n", strerror(errno));
		return;
	}

	if(close(fd) < 0){
		printf("Close error: %s\n", strerror(errno));
		return;
	}

	if(dup2(stdoutFD, STDOUT_FILENO) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}
}

void inputOutputRedirection(char* program[], char* inFile, char* outFile){
	int inFD;
	int outFD;
	int stdoutFD;
	int stdinFD;
	if((stdinFD = dup(STDIN_FILENO)) < 0){
		printf("dup error: %s\n", strerror(errno));
		return;
	}
	if((stdoutFD = dup(STDOUT_FILENO)) < 0){
		printf("dup error: %s\n", strerror(errno));
		return;
	}

	if((inFD = open(inFile, O_RDONLY)) < 0){
		printf("Open error: %s\n", strerror(errno));
		return;
	}
	if((outFD = open(outFile, O_WRONLY)) < 0){
		printf("Open error: %s\n", strerror(errno));
		return;
	}

	if(dup2(outFD, STDOUT_FILENO) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}
	if(dup2(inFD, STDIN_FILENO) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}

	if(sfish_exec(program) < 0){
		printf("exec error: %s\n", strerror(errno));
		return;
	}

	if(close(inFD) < 0){
		printf("Close error: %s\n", strerror(errno));
		return;
	}
	if(close(outFD) < 0){
		printf("Close error: %s\n", strerror(errno));
		return;
	}

	if(dup2(stdoutFD, STDOUT_FILENO) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}
	if(dup2(stdinFD, STDIN_FILENO) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}
}

void singlePipe(char* program1[], char* program2[]){
	int pipefd[2];
	int stdoutFD;
	int stdinFD;
	if((stdinFD = dup(STDIN_FILENO)) < 0){
		printf("dup error: %s\n", strerror(errno));
		return;
	}
	if((stdoutFD = dup(STDOUT_FILENO)) < 0){
		printf("dup error: %s\n", strerror(errno));
		return;
	}

	if((pipe(pipefd)) < 0){
		printf("pipe error: %s\n", strerror(errno));
		return;
	}

	//program1
	//change stdout
	if((dup2(pipefd[1], STDOUT_FILENO)) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if((sfish_exec(program1)) < 0){
		printf("execv error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	//restory stdout
	if((dup2(stdoutFD, STDOUT_FILENO)) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}

	if((close(pipefd[1])) < 0){
		printf("close error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	//program2
	//change stdin
	if((dup2(pipefd[0], STDIN_FILENO)) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if((sfish_exec(program2)) < 0){
		printf("execv error: %s\n", strerror(errno));
		return;
	}

	//restory stdout
	if((dup2(stdinFD, STDIN_FILENO)) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}

	if((close(pipefd[0])) < 0){
		printf("close error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void doublePipe(char* program1[], char* program2[], char* program3[]){
	int pipefd1[2];
	int pipefd2[2];
	int stdoutFD;
	int stdinFD;
	if((stdinFD = dup(STDIN_FILENO)) < 0){
		printf("dup error: %s\n", strerror(errno));
		return;
	}
	if((stdoutFD = dup(STDOUT_FILENO)) < 0){
		printf("dup error: %s\n", strerror(errno));
		return;
	}

	if((pipe(pipefd1)) < 0){
		printf("pipe error: %s\n", strerror(errno));
		return;
	}
	if((pipe(pipefd2)) < 0){
		printf("pipe error: %s\n", strerror(errno));
		return;
	}

	//first program
	//change stdout
	if((dup2(pipefd1[1], STDOUT_FILENO)) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if((sfish_exec(program1)) < 0){
		printf("execv error: %s\n", strerror(errno));
		return;
	}

	//close used ends of pipes
	if((close(pipefd1[1])) < 0){
		printf("close error: %s\n", strerror(errno));
		return;
	}

	//program2
	//change stdout
	if((dup2(pipefd2[1], STDOUT_FILENO)) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}
	//change stdin
	if((dup2(pipefd1[0], STDIN_FILENO)) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}

	if((sfish_exec(program2)) < 0){
		printf("execv error: %s\n", strerror(errno));
		return;
	}

	//close used ends of pipes
	if((close(pipefd1[0])) < 0){
		printf("close error: %s\n", strerror(errno));
		return;
	}
	if((close(pipefd2[1])) < 0){
		printf("close error: %s\n", strerror(errno));
		return;
	}

	//program3

	//change stdin
	if((dup2(pipefd2[0], STDIN_FILENO)) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}

	//restore stdout
	if((dup2(stdoutFD, STDOUT_FILENO)) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}

	if((sfish_exec(program3)) < 0){
		printf("execv error: %s\n", strerror(errno));
		return;
	}

	//close unused ends of pipes
	if((close(pipefd2[0])) < 0){
		printf("close error: %s\n", strerror(errno));
		return;
	}

	//restore stdin
	if((dup2(stdinFD, STDIN_FILENO)) < 0){
		printf("dup2 error: %s\n", strerror(errno));
		return;
	}
}
