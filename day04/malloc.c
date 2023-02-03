/*
 * 2023-02-01
 * djh-sudo
 * malloc.c
*/
#include "minicrt.h"
#define HEAP_SIZE (1 << 25)

typedef struct _heap_header{
    enum{
        HEAP_BLOCK_FREE = 0xABABABAB,
        HEAP_BLOCK_USED = 0xCDCDCDCD,
    }type;
    unsigned int size;
    struct _heap_header* prev;
    struct _heap_header* next;
} heap_header;

#define ADDR_ADD(addr, offset) (((char *)(addr)) + offset)
#define HEADER_SIZE sizeof(heap_header)

static heap_header* list_head = NULL;

static int brk(void* end_data_seg){
    // system call
    int ret = 0;
    asm volatile("movl $45, %%eax\n\t"
                 "movl %1, %%ebx\n\t"
                 "int $0x80\n\t"
                 "movl %%eax, %0\n\t"
                 :"=r"(ret)
                 :"m"(end_data_seg));
    return ret;
}

void free(void* ptr){
    heap_header* header = (heap_header*)ADDR_ADD(ptr, -HEADER_SIZE);
    if(header->type != HEAP_BLOCK_USED){
        // bad free!
        return;
    }
    header->type = HEAP_BLOCK_FREE;
    // merge in front of this block
    if(header->prev != NULL && header->prev->type == HEAP_BLOCK_FREE){
        header->prev->next = header->next;
        if(header->next != NULL){
            header->next->prev = header->prev;
        }
        header->prev->size += header->size;
        header = header->prev;
    }
    // merge the folloing block
    if(header->next != NULL && header->next->type == HEAP_BLOCK_FREE){
        header->size += header->next->size;
        header->next = header->next->next;
    }
}

void* malloc(unsigned int size){
    heap_header* header = list_head;
    if(size == 0 || size >= HEAP_SIZE){
        // bad alloc
        return NULL;
    }
    while(header != NULL){
        if(header->type == HEAP_BLOCK_USED){
            header = header->next;
            continue;
        }
        if(header->size > size + HEADER_SIZE &&
           header->size <= size + (HEADER_SIZE << 1)){
            // can't alloc this block
            header->type = HEAP_BLOCK_USED;
        }
        if(header->size > size + (HEADER_SIZE << 1)){
            // split the block
            heap_header* next = (heap_header*) ADDR_ADD(header, size + HEADER_SIZE);
            next->prev = header;
            next->next = header->next;
            next->type = HEAP_BLOCK_FREE;
            next->size = header->size - (size + HEADER_SIZE);
            // setting the header info
            header->next = next;
            header->size = size + HEADER_SIZE;
            header->type = HEAP_BLOCK_USED;
            return ADDR_ADD(header, HEADER_SIZE);
        }
        header = header->next;
    }
    return NULL;
}

int mini_crt_heap_init(){
    void* base = NULL;
    heap_header* header = NULL;
    
    base = (void*)brk(0);
    void* end = ADDR_ADD(base, HEAP_SIZE);
    end = (void*)brk(end);
    if(end == NULL){
        return 0;
    }
    header = (heap_header*) base;
    // setting the header
    header->type = HEAP_BLOCK_FREE;
    header->size = HEAP_SIZE;
    header->next = NULL;
    header->prev = NULL;
    list_head = header;
    return 1;
}