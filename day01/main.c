#include "minicrt.h"

int main(int argc, char* argv[]){
    char* p = malloc(0x100);
    free(p);
    return 0;
}