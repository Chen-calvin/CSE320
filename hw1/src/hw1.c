#include "hw1.h"
#include <stdio.h>
// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

char validargs(int argc, char** argv, FILE** in, FILE** out) {
	char ret = 0;

	char* HELPFLAG = "-h";//, SUBFLAG = "-s", TUTFLAG = "-t";
	if(argc < 2 || argc > 5){ //error if # of args < 1 or > 5
		return ret;
	}

	char* arg1 = *(argv + 1);

	if((*arg1 == *HELPFLAG) && (*arg1+1 == *HELPFLAG+1))
		ret = ret | 0x80;
	return ret;
}
