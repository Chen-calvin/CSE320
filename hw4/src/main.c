#include "sfish.h"
#include "debug.h"
#include "builtin.h"
#include "executable.h"

/*
 * As in previous hws the main function must be in its own file!
 */

int main(int argc, char const *argv[], char* envp[]){
    /* DO NOT MODIFY THIS. If you do you will get a ZERO. */
    rl_catch_signals = 0;
    /* This is disable readline's default signal handlers, since you are going to install your own.*/
    char* cmd;
    //char cmd[BUFFER_SIZE];
    char* netID = "<haoychen> : <";
    char *prompt = malloc(PATH_MAX * sizeof(char));
    strcpy(prompt, netID);
    strcat(prompt, getenv("PWD"));
    strcat(prompt,"> $");
    while((cmd = readline(prompt)) != NULL) {
        //strcpy(cmd, "echo hehe\n");
        if (strcmp(cmd, "exit") == 0)
            exit(EXIT_SUCCESS);
        //printf("%s\n",cmd);

        char** args = getArgs(cmd);
        if(strcmp(args[0], "help") == 0)
            help();
        else if(strcmp(args[0], "pwd") == 0)
            pwd();
            else if(strcmp(args[0], "cd") == 0){
                if(args[1] == NULL)
                    cd(" ");
                else
                    cd(args[1]);
            }
            else{
                sfish_exec(args);
            }

        /* All your debug print statements should use the macros found in debu.h */
        /* Use the `make debug` target in the makefile to run with these enabled. */
        info("Length of command entered: %ld\n", strlen(cmd));
        /* You WILL lose points if your shell prints out garbage values. */

        strcpy(prompt, netID);
        strcat(prompt, getenv("PWD"));
        strcat(prompt,"> $");
    }

    /* Don't forget to free allocated memory, and close file descriptors. */
    free(cmd);
    free(prompt);

    return EXIT_SUCCESS;
}
