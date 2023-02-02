/*
 * 2023-02-01
 * djh-sudo
 * entry.c
*/
#include "minicrt.h"
extern int main(int argc, char* argv[]);

void exit(int /* exit code */);

static void crt_fatal_error(const char* msg){
    // print(msg)
    exit(1);
}

void mini_crt_entry(void){
    int ret;
    int argc;
    char** argv;
    char* ebp_reg = NULL;

    // %ebp = ebp_reg
    asm volatile("movl %%ebp, %0\n"
                 :"=r"(ebp_reg));
    argc = *(int*)(ebp_reg + 4);
    argv = (char**)(ebp_reg + 8);
    // init heap
    if(mini_crt_heap_init() == 0){
        crt_fatal_error("heap initialize failed!");
    }
    // init I/O
    // add in day02
    if(mini_crt_io_init() == 0){
        crt_fatal_error("heap initialize failed!");
    }
    ret = main(argc, argv);
    exit(ret);
}

void exit(int exit_code){
    asm volatile("movl %0, %%ebx\n\t"
                 "movl $1, %%eax\n\t"
                 "int $0x80\n\t"
                 "hlt\n\t"
                 ::"m"(exit_code));
}