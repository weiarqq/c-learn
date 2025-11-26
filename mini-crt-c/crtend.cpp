// /crtend.cpp
#ifndef WIN32
typedef void (*ctor_func)(void);
// ctor_func crt_end[1]：定义一个长度为 1 的数组，元素类型是 ctor_func（函数指针）。 指向链表末尾
// __attribute__ ((section(".ctors")))：GCC 扩展属性，强制将该数组放入目标文件的 .ctors 段
ctor_func crt_end[1] __attribute__ ((section(".ctors"))) =
{
    (ctor_func) -1
};
#endif