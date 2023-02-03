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
// add in day03
int strcmp(const char* src, const char* dst);
char* strcpy(char* dst, const char* src);
unsigned int strlen(const char* str);
char* itoa(int n, char* str, int radix);

// file and I/O
typedef int FILE;
#define EOF -1

int mini_crt_io_init();
FILE* fopen(const char* pathname, const char* mode);
int fread(void* buffer, int size, int count, FILE* stream);
int fwrite(void* buffer, int size, int count, FILE* stream);
int fclose(FILE* fp);
int fseek(FILE* fp, int offset, int set);

// C++
#ifdef __cplusplus
}
#endif

#endif // __MINI_CRT_H__