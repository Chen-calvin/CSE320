#include "hw1.h"
#include <stdio.h>
// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

char* HELPFLAG = "-h";
char* SUBFLAG = "-s";
char* TUTFLAG = "-t";
char* ENFLAG = "-e";
char* DEFLAG = "-d";
char DASH = '-';

int stringLength(char* str){
	int length = 0;
	char currChar = '*';
	for(length = 0; currChar != '\0'; length++)
		currChar = *(str+length);

	return length - 1;
}

int validIntString(char* str){
	char currChar = *str;
	int i = 1;
	for(i = 1; currChar != '\0'; i++){
		if((int)currChar > 57 || (int)currChar < 48)
			return FALSE;
		currChar = *(str+i);
	}

	return TRUE;
}

int stringToInt(char* str)
{
	int val = 0;

	int i = 0;
	char currChar = *str;
	for(i = 1; currChar != '\0'; i++){
		val = (val * 10) + (int)currChar - 48;
		currChar = *(str+i);
	}

	return val;
}

int checkFlag(char* str1, char* str2){
	int validFlag = FALSE;
	if((*str1 == *str2) && (*(str1+1) == *(str2+1)))
		validFlag = TRUE;

	return validFlag;
}

char validargs(int argc, char** argv, FILE** in, FILE** out) {
	char ret = 0;

	if(argc != 2 && argc != 5 && argc != 6){ //error if # of args < 2 or > 6
		printf("Wrong args \n");
		return 0;
	}

	char* arg1 = *(argv + 1);		//Help, sub, or tut
	if(checkFlag(arg1, HELPFLAG) && argc == 2)
		return 0x80;
	else if(checkFlag(arg1, SUBFLAG) && (argc == 5 || argc == 6)){
		ret = ret | 0x40;
		}
		else if(checkFlag(arg1, TUTFLAG) && argc == 5)
			ret = ret | 0x00;
			else
				return 0;

	char* arg2 = *(argv + 2);	//Encode or decode
	if(checkFlag(arg2, DEFLAG))
		ret = ret | 0x20;
	else if(checkFlag(arg2, ENFLAG))
		ret = ret | 0x00;
		else
			return 0;

	char* arg3 = *(argv + 3);	//input path
	if(*arg3 == DASH)
		*in = stdin;
	else
		*in = fopen(arg3, "r");

	char* arg4 = *(argv + 4);	//output path
	if(*arg4 == DASH)
		*out = stdout;
	*out = fopen(arg4, "w");

	char* arg5;
	int shiftVal;
	if(argc == 6){
		arg5 = *(argv+5);
		if(validIntString(arg5)){
			shiftVal = stringToInt(arg5) % stringLength(Alphabet);
		}
		else
			return 0;
	}
	else
		shiftVal = 320 % stringLength(Alphabet);

	ret = ret | shiftVal;
	return ret;
}
