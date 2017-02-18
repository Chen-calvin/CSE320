#include "hw2.h"
#include "mispelling.h"
#include <ctype.h>

/* Great filename. */

void processDictionary(FILE* f){
    dict->num_words = 0;
    char line[MAX_SIZE];
    fgets(line, MAX_SIZE, f);
    while(!feof(f))
    {
        //initialize the current word.
        struct dict_word* currWord;
        if((currWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
        {
            printf("OUT OF MEMORY.\n");
            return;
        }
        currWord->num_misspellings = 0;
        currWord->misspelled_count = 0;

        //variables
        char word[MAX_SIZE];
        char* wdPtr = word;
        char* character = line;
        //char word_list[MAX_MISSPELLED_WORDS+1][MAX_SIZE];
        int counter = 0;
        int firstWord = 1;

        //if there isn't a space at the end of the line, put one there
        if((line[strlen(line)-2] != ' ' && line[strlen(line)-1] == '\n') || (line[strlen(line)-1] != ' ' && line[strlen(line)-1] != '\n'))
            strcat(line, " ");

        while(*character != '\0')
        {
            if(counter > MAX_MISSPELLED_WORDS)
                break;
            //if the character is a space, add the word in word_list and make word NULL.
            if(*character == ' ')
            {
                *(wdPtr++) = '\0';
                wdPtr = word;
                if(firstWord)
                {
                    addWord(currWord, wdPtr);
                    dict->num_words++;

                    firstWord = 0;
                }
                else
                {
                    struct misspelled_word* currMisspelling;
                    if((currMisspelling = malloc(sizeof(struct misspelled_word))) == NULL)
                    {
                        printf("ERROR: OUT OF MEMORY.");
                        return;
                    }

                    addMisspelledWord(currMisspelling, currWord, wdPtr);
                }
            }
            //if the character isn't a space or a new line, add the character to word.
            else if(*character != '\n')
                *(wdPtr++) = *character;
            character++;
        }
        fgets(line, MAX_SIZE, f);
    }
}

void addWord(struct dict_word* dWord, char* word){
    //setting up dWord fields
    dWord->misspelled_count = 0;
    dWord->num_misspellings = 0;
    dWord->next = dict->word_list;
    strcpy(dWord->word, word);
    dict->word_list = dWord;
}

void addMisspelledWord(struct misspelled_word* misspelledWord, struct dict_word* correctWord, char* word){
    //setting up misspelledWord fields
    strcpy(misspelledWord->word, word);
    misspelledWord->misspelled = 0;
    misspelledWord->correct_word = correctWord;
    misspelledWord->next = m_list;
    (correctWord->misspelled)[correctWord->num_misspellings] = misspelledWord;
    correctWord->num_misspellings++;
    m_list = misspelledWord;
}

void freeWords(struct dict_word* currWord){
    if(currWord != NULL){
        freeWords(currWord->next);
        freeMisspelledWords((currWord->misspelled)[(currWord->num_misspellings)-1]);
        //printf("FREED %s\n", currWord->word);
        free(currWord);
    }
}

void freeMisspelledWords(struct misspelled_word* currWord){
    if(currWord != NULL){
        freeMisspelledWords(currWord->next);
        //printf("FREED %s\n", currWord->word);
        free(currWord);
    }
}

void printWords(struct dict_word* currWord, FILE** f){
    if(currWord != NULL)
    {
        printWords(currWord->next, f);

        int i;

        fprintf(*f, "%s", currWord->word);
        for(i = 0; i<currWord->num_misspellings; i++)
        {
            char* tmp = (currWord->misspelled)[i]->word;
            fprintf(*f, " %s", tmp);
            //fprintf(*f, " %s", tmp);
        }
        fprintf(*f, "\n");
    }
}

void processWord(char* inputWord, int A, FILE** output){
    char lPunc[MAX_SIZE];
    char actualWord[MAX_SIZE];
    char tPunc[MAX_SIZE];
    //Need to split "inputWord" into leading punctuations, lowercase actual word, and trailing punctuations

    int fLetter = fLetterPos(inputWord);
    int lLetter = lLetterPos(inputWord);

    if(fLetter == -1 && lLetter == -1){
        fprintf(*output, "%s", inputWord);
        return;
    }

    int lPuncLen = -1;
    if(fLetter == 0 || fLetter == -1)
        lPunc[0] = '\0';
    else{
        lPuncLen = fLetter;
        int i;
        for(i = 0; i < lPuncLen; i++)
            lPunc[i] = inputWord[i];
        lPunc[i+1] = '\0';
    }

    int tPuncLen = -1;
    int num = strlen(inputWord);
    if(lLetter == num - 1)
        tPunc[0] = '\0';
    else{
        tPuncLen = strlen(inputWord) - lLetter - 1;
        int i;
        for(i = 0; i < tPuncLen; i++)
            tPunc[i] = inputWord[lLetter + i + 1];
        tPunc[i + 1] = '\0';
    }

    int wordLen = lLetter - fLetter + 1;
    if(wordLen == 1){
        int i;
        for(i = 0; i < wordLen; i++)
            actualWord[i] = tolower(inputWord[i]);
        actualWord[i + 1] = '\0';
    }
    else{
        int i;
        for(i = 0; i < wordLen; i++)
            actualWord[i] = tolower(inputWord[i + fLetter]);
        actualWord[i + fLetter] = '\0';
    }

    char* correctWord = foundMisspelledMatch(actualWord);
    if(strcmp(correctWord, "\0") != 0)
        (void)0;
    else if(foundDictMatch(actualWord))
            correctWord = actualWord;
        else
        {
            if(A != -1)
            {
                struct dict_word* newWord;
                //int counter = 0;

                if((newWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
                {
                    printf("ERROR: OUT OF MEMORY.\n");
                    return;
                }

                addWord(newWord, actualWord);
                dict->num_words++;

                char** typos = gentypos(A, actualWord);
                for(int i = 0; i < A; i++){
                    struct misspelled_word* currMWord;
                        if((currMWord = malloc(sizeof(struct misspelled_word))) == NULL)
                        {
                            printf("ERROR: OUT OF MEMORY.");
                            return;
                        }
                    addMisspelledWord(currMWord, newWord, typos[i]);
                }
                correctWord = actualWord;
            }
        }

    fprintf(*output, "%s%s%s", lPunc, correctWord, tPunc);

    int correctWordLen = strlen(correctWord);
    memset(actualWord, '\0', correctWordLen);
    memset(correctWord, '\0', correctWordLen);
}

char* foundMisspelledMatch(char* inputWord){
    struct misspelled_word* listPtr = m_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
        {
            strcpy(inputWord, listPtr->correct_word->word);
            listPtr->misspelled = 1;
            listPtr->correct_word->misspelled_count++;
            return listPtr->correct_word->word;
        }
        listPtr = listPtr->next;
    }
    return "\0";
}

bool foundDictMatch(char* inputWord){
    struct dict_word* listPtr = dict->word_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
            return true;
        listPtr = listPtr->next;
    }
    return false;
}

int fLetterPos(char* word){
    int num = strlen(word);
    for(int i = 0; i < num; i++){
        if(((int)(*(word+i)) < 123 && (int)(*(word+i)) > 96) || ((int)(*(word+i)) < 91 && (int)(*(word+i)) > 64))
            return i;
    }

    return -1;
}

int lLetterPos(char* word){
    int num = strlen(word);
    for(int i = num - 1; i >= 0; i--){
        if(((int)(*(word+i)) < 123 && (int)(*(word+i)) > 96) || ((int)(*(word+i)) < 91 && (int)(*(word+i)) > 64))
            return i;
    }

    return -1;
}

int misspelledWordNum(struct dict_word* head){
    int numMWords = 0;
    while(head != NULL){
        numMWords += head->num_misspellings;
        head = head->next;
    }

    return numMWords;
}
