#ifndef __MINI_CRT_H__
#define __MINI_CRT_H__
// C++
#ifdef __cplusplus
extern "C"{
#endif
// nullptr
#ifndef NULL
#define NULL (0)
#endif

// heap
void free(void* ptr);
void* malloc(unsigned int size);
int mini_crt_heap_init();

// string
int strcmp(const char* src, const char* dst);
char* strcpy(char* dst, const char* src);
unsigned int strlen(const char* str);
char* itoa(int n, char* str, int radix);

// file and I/O
typedef int FILE;
#define EOF -1

#define stdin  ((FILE*)0)
#define stdout ((FILE*)1)
#define stderr ((FILE*)2)

int mini_crt_io_init();
FILE* fopen(const char* pathname, const char* mode);
int fread(void* buffer, int size, int count, FILE* stream);
int fwrite(const void* buffer, int size, int count, FILE* stream);
int fclose(FILE* fp);
int fseek(FILE* fp, int offset, int set);

// add in day04
#define va_list char*
#define va_start(ap, arg) (ap = (va_list) &arg + sizeof(arg))
#define va_arg(ap, type) (*(type*)((ap+=sizeof(type)) - sizeof(type)))
#define va_end(ap) (ap=(va_list)0)
// I/O
int fputc(int c, FILE* stream);
int fputs(const char* str, FILE* stream);
int vfprintf(FILE* stream, const char* format, va_list arglist);
int printf(const char* format, ...);
int fprintf(FILE* stream, const char* format, ...);
// C++
#ifdef __cplusplus
}
#endif

#endif // __MINI_CRT_H__