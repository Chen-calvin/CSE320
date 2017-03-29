#include "sfish.h"

char** getArgs(char* cmd){
	int bufferSize = 64, pos = 0;
	char* delimiters = " \n";
	char** tokens = malloc(bufferSize * sizeof(char*));
	if(tokens == NULL){
		printf("malloc error: %s\n", strerror(errno));
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
