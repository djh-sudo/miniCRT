# day04 I/O part2

`06 Feb 2023`

----
## 标准输出
这一部分重点是实现`C`语言的标准输出`printf`函数的一个简化版。
```C
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
```
首先利用`fwrite`，可以封装出`fputc`和`fputs`，分别表示向文件流中输出一个字符和字符串。
接着，由于`printf`是可变参数，其本质是对`vfprintf`的一个封装，因此核心部分在于实现`vfprintf`。
函数一共有三个参数，分别表示流、输出格式和输出参数，这里`va_list`本质就是`char*`。
```C
int vfprintf(FILE* stream, const char* format, va_list arglist);
```
这里需要提前封装一个宏，将他们放在头文`minicrt.h`件中，
```C
#define va_list char*
#define va_start(ap, arg) (ap = (va_list) &arg + sizeof(arg))
#define va_arg(ap, type) (*(type*)((ap+=sizeof(type)) - sizeof(type)))
#define va_end(ap) (ap=(va_list)0)
```
这样做的目的是便于后续的不定参数的解析。`vfprintf`这里主要对`%d`、`%s`这两种格式进行解析，实际的`printf`还会复杂很多，因为涉及各种格式的解析，这里进行了简化；实现的思路是：对`format`采用状态机的方式，分析输出是转义字符类型还是直接输出类型。
```C
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
```
这里的返回值是输出字符的数量，该函数需要字符串处理函数的支持。这里我们用到了`va_arg(ap, type)`宏，其作用是，返回指针(地址)，并且`ap`根据具体的数据类型(`type`)向后偏移对应的大小。

## 一个小`bug`
我们的`demo`程序如下:
```C
#include "minicrt.h"
int main(int argc, char* argv[]){
    int i = 0;
    for(; i < argc; ++i){
        printf("arg %d is %s\n", i, argv[i]);
    }
    return 0;
}
```
当我们运行这段程序时，会出现`segment fault`。应该是访问了错误的内存，由于源码级调试在第一个`printf`函数返回后出现崩溃，无法具体原因，因此我们需要切换到汇编进行调试，使用`gdb`，发现崩溃点在`for`循环比较的指令上。
这里如果将`argc`更换为其他变量/常量，就不会发生崩溃，原因是：
`argc`的地址被存储在了`ebx`寄存器中，而`printf`函数对`ebx`寄存器进行了修改，修改前后并没有保存上下文，因此一旦调用`printf`后，`ebx`寄存器的值将变为`1`，此时`cmp (%ebx), xxx`，显然这个地址是非法的，因此会出现`segment fault`。

这里需要在`write`系统调用的实现部分进行修改：
```C
asm volatile("movl $4, %%eax\n\t"
                 CALL_PARAMETER
                 :"=m"(ret)
                 :"m"(fd), "m"(buffer), "m"(size)
                 // this will saing the %ebx!
                 :"%ebx");
    return ret;
```
这里增加了最后一个参数，也就是被“破坏”的寄存器，我们将`ebx`添加了进去，这样编译器对于`ebx`寄存器会进行上下文保护，在调用`printf`前后会有`push/pop`的操作。

对于其他内联汇编，对相同的问题进行了一并修改，

## 编译运行
```bash
make run
cd build/
./main 1 2 3 4 5 6 7 8
```