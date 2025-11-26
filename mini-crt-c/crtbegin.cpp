// /crtbegin.cpp
#ifndef WIN32
typedef void (*ctor_func)(void); // 函数指针类型 无参数无返回值类型的函数
// ctor_func ctors_begin[1]：定义一个长度为 1 的数组，元素类型是 ctor_func（函数指针）。
// __attribute__ ((section(".ctors")))：GCC 扩展属性，强制将该数组放入目标文件的 .ctors 段
ctor_func ctors_begin[1] __attribute__ ((section(".ctors"))) =
{
    // 将数组唯一元素设为 -1（强制转换为函数指针类型），作为 链表的起始哨兵
    (ctor_func) -1
};

void run_hooks()
{
    const ctor_func* list = ctors_begin;    // 1. 指向链表起始哨兵
    while (((long)*++list) != -1)           // 2. 遍历链表，跳过起始哨兵，直到遇到结束哨兵
        (**list)();                         // 3. 执行当前构造函数
}
#endif