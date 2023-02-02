/*
 * 2023-02-02
 * djh-sudo
 * string.c
*/

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

char* strcpy(char* dst, const char* src) {
    char* res = dst;
    while(*src){
        *dst++ = *src++;
    }
    *dst = '\0';
    return res;
}

unsigned int strlen(const char* str){
    unsigned int cnt = 0;
    if(!str) return 0;
    while(*str){
        ++str;
        ++cnt;
    }
    return cnt;
}