/*
 * 2023-02-01
 * djh-sudo
 * stdio.c
*/
#include "minicrt.h"

// from /usr/include/bits/fcntl.h
#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR   02
#define O_CREAT  0100
#define O_TRUNC  01000
#define O_APPEND 02000


#define CALL_PARAMETER "movl %1, %%ebx\n\t" \
                 "movl %2, %%ecx\n\t"       \
                 "movl %3, %%edx\n\t"       \
                 "int $0x80\n\t"            \
                 "movl %%eax, %0\n\t"       \

int mini_crt_io_init() {
    return 1;
}

static int open(const char* path, int flag, int mode) {
    int fd = 0;
    asm volatile("movl $5, %%eax\n\t"
                 CALL_PARAMETER
                 :"=m"(fd)
                 :"m"(path), "m"(flag), "m"(mode));
    return fd;
}

static int read(int fd, void* buffer, unsigned int size) {
    int ret = 0;
    asm volatile("movl $3, %%eax\n\t"
                 CALL_PARAMETER
                 :"=m"(ret)
                 :"m"(fd), "m"(buffer), "m"(size));
    return ret;  
}

static int write(int fd, const void* buffer, unsigned int size) {
    int ret = 0;
    asm volatile("movl $4, %%eax\n\t"
                 CALL_PARAMETER
                 :"=m"(ret)
                 :"m"(fd), "m"(buffer), "m"(size));
    return ret;
}

static int seek(int fd, int offset, int mode) {
    int ret = 0;
    asm volatile("movl $19, %%eax\n\t"
                  CALL_PARAMETER
                  :"=m"(ret)
                  :"m"(fd), "m"(offset), "m"(mode));
    return ret;
}

static int close(int fd) {
    int ret = 0;
    asm volatile("movl $6, %%eax\n\t"
                 "movl %1, %%ebx\n\t"
                 "int $0x80\n\t"
                 "movl %%eax, %0\n\t"
                 :"=m"(ret)
                 :"m"(fd));
    return ret;
}
// C standard API
FILE* fopen(const char* filename, const char* mode) {
    int fd = -1;
    int flags = 0;
    int access = 00700;
    // add in day03
    if(strcmp(mode, "w") == 0) flags |= O_WRONLY | O_CREAT | O_TRUNC;

    if(strcmp(mode, "w+") == 0) flags |= O_RDWR | O_CREAT | O_TRUNC;

    if(strcmp(mode, "r") == 0) flags |= O_RDONLY;

    if(strcmp(mode, "r+") == 0) flags |= O_RDWR | O_CREAT;

    fd = open(filename, flags, access);
    if(fd == -1) return NULL;
    return (FILE*)fd;
}

int fread(void* buffer, int size, int count, FILE* stream) {
    return read((int)stream, buffer, size * count);
}

int fwrite(const void* buffer, int size, int count, FILE*stream) {
    return write((int)stream, buffer, size * count);
}

int fclose(FILE* fp){
    return close((int) fp);
}

int fseek(FILE *fp, int offset, int set) {
    return seek((int)fp, offset, set);
}

// add day04
int fputc(int c, FILE* stream) {
    if(fwrite(&c, 1, 1, stream) != 1){
        return EOF;
    }else{
        return c;
    }
}

int fputs(const char* str, FILE* stream) {
    int len = strlen(str);
    if(fwrite(str, 1, len, stream) != len){
        return EOF;
    }else{
        return len;
    }
}

int vfprintf(FILE* stream, const char* format, va_list arglist) {
    int translate = 0;
    int ret = 0;
    const char* p = format;
    for(; *p != NULL; ++p) {
        switch(*p) {
            case '%':{
                if(translate == 0) translate = 1;
                else{
                    if(fputc('%', stream) < 0) return EOF;
                    ++ret;
                    translate = 0;
                }
                break;
            }
            case 'd':{
                if(translate){
                    char buf[16] = { 0 };
                    translate = 0;
                    itoa(va_arg(arglist, int), buf, 10);
                    if(fputs(buf, stream) < 0) return EOF;
                    ret += strlen(buf);
                }else if(fputc('d', stream) < 0) return EOF;
                else ++ret;
                break;
            }
            case 's':{
                if(translate){
                    const char* str = va_arg(arglist, const char*);
                    translate = 0;
                    if(fputs(str, stream) < 0) return EOF;
                    ret += strlen(str);
                }else if(fputc('s', stream) == EOF) return EOF;
                else ++ret;
                break;
            }
            default:{
                if(translate) translate = 0;
                if(fputc(*p, stream) < 0) return EOF;
                else ++ret;
                break;
            }
        }
    }
    return ret;
}

int printf(const char* format, ...) {
    va_list(arglist);
    va_start(arglist, format);
    return vfprintf(stdout, format, arglist);
}

int fprintf(FILE* stream, const char* format, ...) {
    va_list(arglist);
    va_start(arglist, format);
    return vfprintf(stream, format, arglist);
}