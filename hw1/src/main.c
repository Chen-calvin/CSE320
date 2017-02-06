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

    int n = (int)(ret & 0x1F);
    if((ret & 0x40) == 64){
    	if((ret & 0x20) == 32)
    		subDecode(&in, &out, n);
    	else
    		subEncode(&in, &out, n);
    }
    else{
    	if((ret & 0x20) == 32){
    		tutDecode(&in, &out);
        }
    	else{
    		tutEncode(&in, &out);
        }
    }

    fclose(in);
    fclose(out);
    return EXIT_SUCCESS;
}
