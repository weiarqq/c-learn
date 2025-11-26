// ctors.cpp
//MSVC 实现全局构造函数
// typedef void (*)(void) init_func; 函数指针类型 无参数无返回值类型的函数
typedef void (*init_func)(void);
#ifdef WIN32
// 自定义数据段声明
#pragma section(".CRT$XCA",long,read)
#pragma section(".CRT$XCZ",long,read)
// 段边界标记
__declspec(allocate(".CRT$XCA")) init_func ctors_begin[] = { 0 }; // ctors_begin指向 一个 无参数无返回值的函数的地址段的开始 
__declspec(allocate(".CRT$XCZ")) init_func ctors_end[] = { 0 };

extern "C" void do_global_ctors()
{
    init_func *p = ctors_begin;
    while (p < ctors_end)
    {
        if (*p != 0)  // p为指向 ctors_begin的地址， *p 解引用为函数的地址
            (**p)(); // **p即为函数本书
        ++p;
    }
}
#else
void run_hooks();
extern "C" void do_global_ctors()
{
    run_hooks();
}
#endif