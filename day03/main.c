#include "minicrt.h"

int main(int argc, char* argv[]){
    FILE* fp = fopen("test.txt", "w+");
    char* p = malloc(20);
    if(fp != NULL){
        fwrite("hello-miniCRT\n", 14, 1, fp);
        fwrite(itoa(1024, p, 16), 20, 1, fp);
    }else{
        // err open
    }
    free(p);
    fclose(fp);
    return 0;
}