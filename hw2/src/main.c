#include "hw2.h"
#include <getopt.h>

int main(int argc, char *argv[]){
    char DEFAULT_DICT_FILE[]= "dictionary.txt";
    DEFAULT_INPUT = stdin;
    DEFAULT_OUTPUT = stdout;
    //create dictionary
    if((dict = (struct dictionary*) malloc(sizeof(struct dictionary))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }

    if((m_list = (struct misspelled_word*) malloc(sizeof(struct misspelled_word*))) == NULL)
    {
        printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }
    m_list = NULL;

    struct Args args;
    // Set struct default values
    args.h = false;
    args.d = false;
    args.i = false;
    args.o = false;
    args.a = -1;
    strcpy(args.dictFile, DEFAULT_DICT_FILE);
    // Make a loop index
    char line[MAX_SIZE];
    //Declare Files
    FILE* dFile;
    FILE* iFile = DEFAULT_INPUT;
    FILE* oFile = DEFAULT_OUTPUT;

    char* endPtr;
    char opt = 0;
    while((opt = getopt(argc, argv, "h:o:i:d:A:")) != -1){
        switch(opt){
            case 'h' :
                USAGE(EXIT_SUCCESS);
                exit(EXIT_SUCCESS);
            case 'o' :
                args.o = true;
                strcpy(args.output, optarg);
                break;
            case 'i' :
                args.i = true;
                strcpy(args.input, optarg);
                break;
            case 'd' :
                args.d = true;
                strcpy(args.dictFile, optarg);
                break;
            case 'A' :
                args.a = strtol(optarg, &endPtr, 10);
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }
    printf("%s\n", args.output);

    exit(EXIT_SUCCESS);

    if(args.i == true)
        iFile = fopen(args.input, "r");
    if(iFile == NULL){
        USAGE(EXIT_FAILURE);
        exit(EXIT_FAILURE);
    }

    if(args.o == true)
        oFile = fopen(args.output, "w");
    if(oFile == NULL){
        USAGE(EXIT_FAILURE);
        exit(EXIT_FAILURE);
    }

    dFile = fopen(args.dictFile, "r");
    if(dFile == NULL){
        USAGE(EXIT_FAILURE);
        exit(EXIT_FAILURE);
    }
    else
    {
        processDictionary(dFile);

    }

    strcpy(line,"\n--------INPUT FILE WORDS--------\n");
    fwrite(line, strlen(line)+1, 1, oFile);

    while(!feof(iFile))
    {
        char word[MAX_SIZE];
        char* wdPtr = word;
        //char line[MAX_SIZE];
        char* character = line;

        fgets(line, MAX_SIZE+1, iFile);

        //if there isn't a space or newline at the end of the line, put one there
        if((line[strlen(line)-1] != ' ') && (line[strlen(line)-1] != '\n'))
            strcat(line, " ");
        //replaces spaces within a line with new lines
        while(character != NULL)
        {
            if(*character == ' ' || *character == '\n')
            {
                /*char* punct = wdPtr-1;
                    printf("char:%c",punct);
                while(!((*punct>='a' && *punct<='z') || (*punct>='A' && *punct<='Z')))
                {
                    punct--;
                }
                punct++;
                printf("%d", strlen(wdPtr)-strlen(punct));
                */

                //*wdPtr = NULL;
                wdPtr = word;

                processWord(wdPtr);

                strcat(wdPtr, " ");
                fwrite(wdPtr, strlen(wdPtr)+1, 1, oFile);
            }
            else
            {
                *(wdPtr++) = *character;
            }
            character++;
        }

        if(iFile == stdin)
            break;
    }

    strcpy(line, "\n--------DICTIONARY WORDS--------\n");
    fwrite(line, strlen(line)+1, 1, oFile);
    printWords(dict->word_list , oFile);

    //printf("\n--------FREED WORDS--------\n");
    freeWords(dict->word_list);
    //free dictionary
    free(dict);
    //free m_list
    free(m_list);

    //fclose(dFile);
    fclose(iFile);
    fclose(oFile);
    return EXIT_SUCCESS;
}
