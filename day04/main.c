#include "minicrt.h"

int main(int argc, char* argv[]){
    int i = 0;
    for(; i < 100; ++i){
        printf("%d, hello-CRT\n", i);
    }
    return 0;
}