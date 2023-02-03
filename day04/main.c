#include "minicrt.h"

int main(int argc, char* argv[]){
    int i = 0;
    for(; i < argc; ++i){
        printf("arg %d is %s\n", i, argv[i]);
    }
    return 0;
}