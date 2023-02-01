# day01 堆区的实现

`01 Feb 2023`

----
## 堆（Heap）
程序的运行离不开内存，堆`heap`是最重要的一种内存之一，因此`miniCRT`首先考虑实现堆。实际的堆的实现算法会复杂很多，因为涉及分配的性能和空间的利用，作为简化，
* 这里将堆区大小固定在了`32MB`；
* 使用双向链表将其串接，并且链表结构没有优化(结点会有额外的空间占用)
* `linux`使用`brk`系统调用实现堆区的分配和初始化

> `brk`的原理
```
brk系统调用本质是修改进程的数据段的边界，将数据段边界扩大，相当于分配一定的内存，但是这里分配的仅仅是虚存，即这些内存还没有被commit，等待程序真正访问这段内存时，发生缺页中断，此时才会分配物理内存。
```
这里我们只需要实现两个关键函数，一个是`malloc(unsigned int)`，另外一个是`free(void*)`。
链表结点的结构如下:
```c
typedef struct _heap_header{
    enum{
        HEAP_BLOCK_FREE = 0xABABABAB,
        HEAP_BLOCK_USED = 0xCDCDCDCD,
    }type;
    unsigned int size;
    struct _heap_header* prev;
    struct _heap_header* next;
} heap_header;
```
* `1.`使用特殊数字标记两种堆区，同时每个结点(头)需要记录它管理的内存大小(size)，采用**双向链表**的目的是便于合并连续的空闲结点。

* `2.`之后是一些宏定义，便于程序使用
```c
#define ADDR_ADD(addr, offset) (((char *)(addr)) + offset)
#define HEADER_SIZE sizeof(heap_header)
#define HEAP_SIZE (1 << 25)
```
* `3.`紧接着是`free`的实现，由于`day01`没有`I/O`部分初始化，因此无法打印任何调试信息。
这里合并的思路是，看看释放结点的`prev`结点，如果是空闲，则向前合并；看看释放结点的`next`结点，如果空闲，则合并。
```c
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
```
* `4.`接着是`malloc`的实现，这里`malloc`实现的时间复杂度是`O(n)`，每次分配需要遍历这个链表，找到一块合适大小的结点然后分配，这样的效率其实不高，实际操作系统的堆区内存管理会复杂很多，这里简化程度较大。

(1) 注意到每次分配的空间其实并不是`size`，而是`size + HEADER_SIZE`!

(2) 对于用户而言，他只拿到了`size`大小的内存；但是对于`OS`而言，它实际分配出去了更多空间；

(3) 换言之，如果每次`malloc`的大小并不大，但是malloc又很频繁，这样带来的`overhead`是不能够容忍的!

(4) 实际的`malloc`依旧会有这个问题，这些`cookie`其实是记录的分配的数量，因此`free`的时候，只需要指明地址(`ptr`)即可

(5) 实际的`malloc`性能并不差，但是存在这些`cookie`，因此`C++ STL`的分配器对它再做了一次封装，一次分配'大量'内存，然后等长切割给程序使用，这些被切割的内存是没有`cookie`的！
```c
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
```
最后还需要用内联汇编实现`brk`，本质是系统调用，然后在`heap_init`中一次申请`32MB`内存，这便是堆区的初始化。细节见源码部分。
## 编译
使用`Makefile`去编译，这里编译用到了几个重要的选项

* `-fno-builtin` 关闭`gcc`内置函数的功能，避免`gcc`将其展开为函数的内部实现，因为这里是自己编写的函数 
* `-nostdlib` 不使用任何来自`Glibc`的库函数和文件
* `-fno-stack-protector`这是一个安全选项，实际的堆栈会有堆栈保护相关的措施，这里是一个最简化的`CRT`，因此也关闭了相应的堆栈保护。
* 运行`make`。之后编译的目标文件在`./build`目录下
```make
    make clear
    make run
```
