#include "minicrt.h"

int fputc(int c, FILE *stream)
{
    if (fwrite(&c, 1, 1, stream) != 1){
        return EOF;
    }
    else{return c;}
}

int fputs( const char *str, FILE *stream)
{
    int len = strlen(str);
    if (fwrite(str, 1, len, stream) != len){
        return EOF;
    }
    else{
        return len;
    }
}

#ifndef WIN32
// 变长参数列表的指针类型（本质是字符指针，指向栈中参数）
#define va_list char* 
// 初始化 ap，指向 arg 的下一个参数
#define va_start(ap,arg) (ap=(va_list)&arg+sizeof(arg)) 
// 取出当前参数（类型为 t），并移动 ap 拿到第一个参数，然后同时ap指向下一个参数 
// ap= ap+sizeof(t) 
// now_index = ap-sizeof(t)
// now_index = 原始ap地址, ap当前指向下一个函数，一行代码解决
#define va_arg(ap,t) (*(t*)((ap+=sizeof(t))-sizeof(t))) 
// 结束后清空 ap（避免野指针）
#define va_end(ap) (ap=(va_list)0) 
#else
#include <Windows.h>
#endif

int vfprintf(FILE *stream, const char *format, va_list arglist)
{
    int translating = 0;
    int ret = 0;
    const char* p = 0;
    for (p = format; *p != '\0'; ++p)
    {
        switch (*p)
        {
        case '%':
            if (!translating)
                translating = 1;
            else
            {
                if (fputc('%', stream) < 0)
                    return EOF;
                ++ret;
                translating = 0;
            }
            break;
        case 'd':
            if (translating)  //%d 查找int类型参数
            {
            char buf[16];
            translating = 0;
            itoa(va_arg(arglist, int), buf, 10);
            if (fputs(buf, stream) < 0)
                return EOF;
            ret += strlen(buf);
            }
            else if (fputc('d', stream) < 0){
                return EOF;
            }else{
                ++ret;
            }
            break;
        case 's':
            if (translating)  //%s 查找 char类型参数
            {
                const char* str = va_arg(arglist, const char*);
                translating = 0;
                if (fputs(str, stream) < 0)
                    return EOF;
                ret += strlen(str);
            }
            else if(fputc('s', stream) < 0){
                return EOF;
            }else{
                ++ret;
            }
            break;
        default:
            if (translating)
                translating = 0;
            if (fputc(*p, stream) < 0){
                return EOF;
            }else{
                ++ret;
            }
            break;
        }
    }
    return ret;
}

int printf (const char *format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    return vfprintf(stdout, format, arglist);
}

int fprintf (FILE *stream, const char *format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    return vfprintf(stream, format, arglist);
}
            