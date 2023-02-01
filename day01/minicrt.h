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


// C++
#ifdef __cplusplus
}
#endif

#endif // __MINI_CRT_H__