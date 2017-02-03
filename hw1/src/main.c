#include "hw1.h"

int main(int argc, char **argv) {

    FILE* in;
    FILE* out;


    /* Note: create a variable to assign the result of validargs */
    char ret = validargs(argc, argv, &in, &out);
    if((int)ret == 0)
    	USAGE(EXIT_FAILURE);

    if((int)ret == -128)
    	USAGE(EXIT_SUCCESS);

    return EXIT_SUCCESS;
}
