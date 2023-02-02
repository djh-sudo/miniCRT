# day02 I/O part1

`02 Feb 2023`

----
## 文件 I/O
如果不借助操作系统提供的`API`，我们很难做到向磁盘写入数据(数据持久化)。为了支持`C`语言常见的文件读写，例如`fread/fwrite`等函数，`miniCRT`还需要进一步完善相关的服务。

在`linux`操作系统下，这些函数是由`write/read/open`等这类系统调用完成的，由于`linux`将一切都抽象为文件，因此`write`这样的系统调用用途非常广泛，既可以向磁盘中写入数据，也可以向终端中写入数据，甚至网络套接字也是使用它。

因此，第一部分首先使用内联汇编实现这些基本的函数。首先是`write`
```C
static int write(int fd, const void* buffer, unsigned int size) {
    int ret = 0;
    asm volatile("movl $4, %%eax\n\t"
                 "movl %1, %%ebx\n\t"
                 "movl %2, %%ecx\n\t"
                 "movl %3, %%edx\n\t"
                 "int $0x80\n\t"     
                 "movl %%eax, %0\n\t"
                 :"=m"(ret)
                 :"m"(fd), "m"(buffer), "m"(size));
    return ret;
} 
```
这里`write`的系统调用号`4`存储在`%eax`寄存器中，其余三个参数分别存储在`ebx`、`ecx`、`edx`中，最后读取字节的返回值存储在`ret`中。
其余几个系统调用，如`read`、`seek`、`open`、`close`等，系统调用号会不同，其余参数如出一辙。

有了系统调用的接口后，只需要进一步封装`C`语言的`fread`之类的函数，实际的实现中，还会考虑诸如缓冲区、错误处理等问题，这里都进行了简化。

以`fopen(const char*, const char mode)`为例：
```C
FILE* fopen(const char* filename, const char* mode) {
    int fd = -1;
    int flags = 0;
    int access = 0700;
    // ...
    flags |= O_RDWR | O_CREAT;
    fd = open(filename, flags, access);
    return (FILE*)fd;
}
```
注意这里的`FILE`是`int`的一个封装(实际的实现更加复杂，这里被简化了)，需要将这个定义放在头文件中，由于`day02`并没有字符串函数的实现，因此对于文件打开方式，这里还有待完善；对于不同的文件打开方式，`flags`应当有不同的组合值！

其余几个`C`语言的标准库函数实现基本是一行系统调用，见源码。

## 编译
在`Makefile`中添加对`stdio.c`的编译选项，后续的链接也需要做一定修改。
```Makefile
stdio: Makefile stdio.c
	$(GCC) -c $(CFLAGS) stdio.c -o $(BUILD_PATH)/stdio.o
```
这里还需要修改`minicrt.h`，需要添加相应函数的声明，对于模块中被标记为`static`的函数，表明该函数不是全局函数，只在模块内可见，不对外导出。