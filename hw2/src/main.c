#include "hw2.h"
#include <getopt.h>
#include <libgen.h>

int main(int argc, char *argv[]){
    char DEFAULT_DICT_FILE[]= "rsrc/dictionary.txt";
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
    struct misspelled_word* m_list_ptr = m_list;
    m_list = NULL;

    struct Args args;
    // Set struct default values
    args.h = false;
    args.d = false;
    args.i = false;
    args.o = false;
    args.A = -1;
    strcpy(args.dictFile, DEFAULT_DICT_FILE);
    // Make a loop index
    char line[MAX_SIZE];
    //Declare Files
    FILE* dFile;
    FILE* iFile = DEFAULT_INPUT;
    FILE* oFile = DEFAULT_OUTPUT;

    char* endPtr;
    char opt = 0;
    //bool AValue = false;
    while((opt = getopt(argc, argv, "h::o:i:d:A:")) != -1){
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
                //AValue = true;
                args.A = strtol(optarg, &endPtr, 10);
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    if(args.A > 5 || args.A < 0){
        USAGE(EXIT_SUCCESS);
        exit(EXIT_SUCCESS);
    }

    /*if(AValue){
        int num = strtol(argv[argc-1], &endPtr, 10);
        if(num > 5 || num < 0){
            USAGE(EXIT_FAILURE);
            exit(EXIT_FAILURE);
        }
        args.A = num;
    }*/

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


    //printWords(dict->word_list, stdout);
    //exit(EXIT_SUCCESS);
    //freeWords(dict->word_list);
    //free(dict);
    //free(m_list_ptr);
    //exit(EXIT_SUCCESS);

    fgets(line, MAX_SIZE, iFile);
    while(!feof(iFile))
    {
        char word[MAX_SIZE];
        char* wdPtr = word;
        //char line[MAX_SIZE];
        char* character = line;


        //if there isn't a space or newline at the end of the line, put one there
        if((line[strlen(line)-1] != ' ') && (line[strlen(line)-1] != '\n'))
            strcat(line, " ");
        //replaces spaces within a line with new lines
        while(*character != '\0')
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
                memset(wdPtr, '\0', 99);
                wdPtr = word;

                processWord(wdPtr, args.A, &oFile);
                fprintf(oFile, "%c", *character);

                //0 strcat(wdPtr, " ");
                //fwrite(wdPtr, strlen(wdPtr)+1, 1, oFile);
            }
            else
            {
                *(wdPtr++) = *character;
            }
            character++;
        }

        if(iFile == stdin)
            break;

        fgets(line, MAX_SIZE, iFile);
    }

    printf("Total number of words in dictionary: %d\n", dict->num_words);
    printf("Size of dictionary (in bytes): %d\n", ((int)sizeof(struct misspelled_word) * misspelledWordNum(dict->word_list)) + dict->num_words * (int)sizeof(struct dict_word));                            //size of dict + size of all dict_words + size of misspelled word list
    printf("Size of misspelled word list (in bytes): %d\n", (int)sizeof(struct misspelled_word) * misspelledWordNum(dict->word_list));                   //size of misspelled word list
    printf("Total number of misspelled words: %d\n", misspelledWordNum(dict->word_list));                          //num of misspelled words
    printf("Top 3 misspelled words: \n");                                    //top 3 misspelled words

    char* dirPtr = strdup(args.dictFile);
    char* dir = dirname(dirPtr);
    char* name = basename(args.dictFile);
    char* fullPath = strcat(dir, "/new_");
    fullPath = strcat(fullPath, name);
    FILE* newDict = fopen(fullPath, "w");
    printWords(dict->word_list, &newDict);
    free(dirPtr);

    //printf("\n--------FREED WORDS--------\n");
    freeWords(dict->word_list);
    //free dictionary
    free(dict);
    //free m_list
    free(m_list_ptr);

    fclose(dFile);
    fclose(iFile);
    fclose(oFile);
    return EXIT_SUCCESS;
}
