# day03 String

`02 Feb 2023`

----
## 字符串
字符串的处理函数不需要借助操作系统的`API`，这里我们简单实现了几个常见的函数`strcpy/strcmp/strlen`，总体难度不大。
以`strcmp`为例:
```C
int strcmp(const char* src, const char* dst) {
    int ret = 0;
    unsigned char* p1 = (unsigned char *)src;
    unsigned char* p2 = (unsigned char *)dst;
    while(!(ret = *p1 - *p2) && *p2){
        ++p1;
        ++p2;
    }
    if(ret < 0) ret = -1;
    else if(ret > 0) ret = 1;
    return ret;
}
```
`C`语言的`strcmp`有三种返回值，分别是`1`，`-1`和`0`。只有返回`0`的时候，两者是相等，`1`表示前者的`ASCII`要大一些，`-1`表示后者的值要大一些(跳过相等的字符后)。

## `I/O`部分
在`day02`的`stdio`中，还需要修改`fopen`函数，对打开模式部分进行修正:
```C
    if(strcmp(mode, "w") == 0) flags |= O_WRONLY | O_CREAT | O_TRUNC;

    if(strcmp(mode, "w+") == 0) flags |= O_RDWR | O_CREAT | O_TRUNC;

    if(strcmp(mode, "r") == 0) flags |= O_RDONLY;

    if(strcmp(mode, "r+") == 0) flags |= O_RDWR | O_CREAT;
```

## 编译
这里我们对`Makefile`的部分指令进行了修改，便于编译，对所有模块的编译选项都增加了`-ggdb`，这样便于`gdb`源码调试；

同时每次修改完`main`函数后，都需要重新链接(这也是静态链接的一个坏处)，因此把原有的`make run`进行了拆解。
如果`minicrt.a`没有变化，只需要重新编译`main`，在链接即可：
```Makefile
    make main
    make update
```
