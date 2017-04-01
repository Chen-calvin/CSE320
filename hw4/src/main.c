#include "sfish.h"
#include "debug.h"
#include "builtin.h"
#include "executable.h"
#include "redirection.h"
#include "handler.h"
#include <stdlib.h>

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
    strcat(prompt,"> $ ");
    init_signal();
    while((cmd = readline(prompt)) != NULL) {
        //strcpy(cmd, "alarm 3\n");
        if (strcmp(cmd, "exit") == 0)
            exit(EXIT_SUCCESS);
        //printf("%s\n",cmd);
        char* fullcmd = strdup(cmd);
        char** args = getArgs(cmd);
        if((strcmp(args[0], "help") == 0) && ((args[1] == NULL)))
            help();
        else if((strcmp(args[0], "alarm") == 0) && ((args[1] != NULL)))
            sfish_alarm(atoi(args[1]));
            else if((strcmp(args[0], "pwd") == 0) && ((args[1] == NULL)))
                pwd();
                else if((strcmp(args[0], "cd") == 0)){
                        if(args[1] == NULL){
                            cd(" ");
                        }
                        else{
                            cd(args[1]);
                        }
                    }
                    else if(checkRedirection(fullcmd)){
                            redirection(fullcmd);
                        }
                        else
                            sfish_exec(args);

        /* All your debug print statements should use the macros found in debu.h */
        /* Use the `make debug` target in the makefile to run with these enabled. */
        //info("Length of command entered: %ld\n", strlen(cmd));
        /* You WILL lose points if your shell prints out garbage values. */

        strcpy(prompt, netID);
        strcat(prompt, getenv("PWD"));
        strcat(prompt,"> $ ");
        free(fullcmd);
    }

    /* Don't forget to free allocated memory, and close file descriptors. */
    free(cmd);
    free(prompt);

    return EXIT_SUCCESS;
}
