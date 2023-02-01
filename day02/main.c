#include "minicrt.h"

int main(int argc, char* argv[]){
    FILE* fp = fopen("test.txt", "w");
    if(fp != NULL){
        fwrite("hello-miniCRT\n", 14, 1, fp);
    }else{
        // err open
    }
    fclose(fp);
    return 0;
}