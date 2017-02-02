#include "hw1.h"

int main(int argc, char **argv) {

    FILE* in;
    FILE* out;


    /* Note: create a variable to assign the result of validargs */
    char ret = validargs(argc, argv, &in, &out);

    if(ret == 0)
    	USAGE(EXIT_FAILURE);

    if((ret & 0x80) == 0x80)
    	USAGE(0);

    return EXIT_SUCCESS;
}
