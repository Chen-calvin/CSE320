#include "hw1.h"
#include <stdio.h>
// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

int checkFlag(char* str1, char* str2){
	int validFlag = FALSE;
	if((*str1 == *str2) && (*str1+1 == *str2+1))
		validFlag = TRUE;

	return validFlag;
}

char validargs(int argc, char** argv, FILE** in, FILE** out) {
	char ret = 0;

	char* HELPFLAG = "-h";//, SUBFLAG = "-s", TUTFLAG = "-t";
	if(argc < 2 || argc > 6) //error if # of args < 2 or > 6
		return ret;

	char* arg1 = *(argv + 1);

	if(checkFlag(arg1, HELPFLAG))
		return ret | 0x80;

	return ret;
}
