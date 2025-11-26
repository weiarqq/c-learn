
#include "minicrt.h"

typedef struct _heap_header {
    enum {
        HEAP_BLOCK_FREE = 0xABABABAB,
        HEAP_BLOCK_USED = 0xCDCDCDCD
    } type;
    unsigned size;
    struct _heap_header* next;
    struct _heap_header* prev;
} heap_header;

// 转化为char* char指针的步长为 1，作用计算 a的偏移地址
// 地址偏移计算：将指针a（任意类型）转换成char*（1字节步长），再加上偏移o，得到新地址

#define ADDR_ADD(a, o) (((char*)(a)) + o)

// 计算header占用内存空间
#define HEAD_SIZE (sizeof(heap_header)) 

// 堆链表头指针
static heap_header* list_head = NULL;

void free(void* ptr)
{
    // ptr为 malloc返回的地址，数据区的起始地址, ADDR_ADD(ptr, -HEAD_SIZE)即为 ptr所在的 header node的起始地址
    // |header1|data1|->|header2|(@header)data2|->|header3|data3|
    // 假设当前为data2, ptr为data2的起始地址,则header2的地址为 ptr-head_size
    heap_header* header = (heap_header*)ADDR_ADD(ptr, -HEAD_SIZE);

    // 合法性检查（避免非法释放）
    if (header->type != HEAP_BLOCK_USED) {
        return;
    }
    // 标记为空闲
    header->type = HEAP_BLOCK_FREE;

    // 向前合并空闲块 非头部节点且上个节点是空闲的，则合并两个相邻的节点空间
    if(header->prev != NULL && header->prev->type == HEAP_BLOCK_FREE){
        // 删除需要释放的节点，合并到上一个节点
        header->prev->next = header->next;
        if(header->next != NULL){
            header->next->prev = header->prev;
        }
        // 合并后更新空间大小
        header->prev->size += header->size;
        header = header->prev;
    }
    // 向后合并空闲块 下一个节点是空闲的，则合并到当前节点
    if(header->next != NULL && header->next->type == HEAP_BLOCK_FREE){
        header->size += header->next->size;
        header->next = header->next->next;
    }
}

void* malloc(unsigned size){
    heap_header* header;
    if(size == 0) return NULL;
    header = list_head;
    while(header != 0){
        // 查找空闲节点
        if(header->type == HEAP_BLOCK_USED){
            header = header->next;
            continue;
        }

        // 空闲空间 大于请求的空间 且无法创建两个头节点
        // 空闲块大小 > (请求大小 + 头部大小)，但不足以分割出一个新的空闲块（剩余空间不够放一个header）
        if(header->size > size + HEAD_SIZE && header->size <= size + HEAD_SIZE * 2){
            header->type = HEAP_BLOCK_USED;
            return ADDR_ADD(header, HEAD_SIZE);  // 返回数据区地址 header起点+占用空间偏移
        }

        // 空闲空间 大于 请求的空间+头节点空间*2
        if(header->size > size + HEAD_SIZE * 2){
            // 移动到 需要的空间尾部，分割空闲空间为 ｜请求的空间｜(@next)header｜空闲空间
            heap_header* next = (heap_header*)ADDR_ADD(header, size + HEAD_SIZE);
            next->prev = header;
            next->next = header->next;
            next->type = HEAP_BLOCK_FREE;
            next->size = header->size - (size + HEAD_SIZE); // 空闲空间-(请求空间+header占用空间)
            header->next = next;
            header->size = size + HEAD_SIZE;
            header->type = HEAP_BLOCK_USED;
            // 返回数据区地址（当前块头部 + 头部大小）
            return ADDR_ADD(header, HEAD_SIZE);
        }
        // 继续遍历
        header = header->next;
    }
    // 没找到空闲空间
    return NULL;
}

#ifndef WIN32
// Linux brk system call
// 调整进程的堆内存边界（堆的 “顶”）不申请地址，只调整堆顶
static int brk(void* end_data_segment) {
    int ret = 0;
    // brk system call number: 45
    // in /usr/include/asm-i386/unistd.h:
    // #define __NR_brk 45
    // 内联汇编（asm）：直接操作CPU寄存器，触发系统调用
    asm(
        // 1. 把系统调用号45存入eax寄存器（Linux规定brk的调用号是45）
        "movl $45, %%eax\n\t"
        // 2. 把函数参数end_data_segment存入ebx寄存器（系统调用的第一个参数存在ebx）
        "movl %1, %%ebx\n\t"
        // 3. 触发0x80中断（32位Linux的系统调用入口，CPU会切换到内核态）
        "int $0x80\n\t"
        // 4. 把系统调用的返回值（存在eax中）存入ret变量
        "movl %%eax, %0\n\t"
        : "=r" (ret) // 输出操作数：ret是输出变量，"=r"表示用任意通用寄存器存储
        : "m" (end_data_segment) // 输入操作数：end_data_segment是输入变量，"m"表示从内存读取
    );
    return ret;
}
#endif

#ifdef WIN32
#include <Windows.h>
#endif
// 堆初始化
int mini_crt_heap_init()
{
    void* base = NULL;
    heap_header* header = NULL;
    // 32 MB heap size = 1024 * 1024 * 32;
    unsigned heap_size = 1024 * 1024 * 32;
#ifdef WIN32
    // VirtualAlloc：Windows系统的内存分配API（用于申请大块虚拟内存）
    // 参数说明：
    // 0：让系统自动选择堆空间的起始地址（推荐）
    // heap_size：申请的内存大小（32MB）
    // MEM_COMMIT | MEM_RESERVE：内存分配方式（先预留+提交，直接可用）
    // PAGE_READWRITE：内存权限（可读可写，堆内存需要读写权限）
    base = VirtualAlloc(0, heap_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (base == NULL)
        return 0;
#else
    // brk(0) 不调整堆大小，只返回当前堆顶（作为新堆的起始 base）；再调用 brk(end) 把堆顶扩展到 base+32MB，相当于向系统申请了 32MB 连续堆空间。
    // 第一步：调用brk(0)获取当前堆顶地址（program break），作为新堆的起始地址base
    base = (void*)brk(0);
    // 第二步：计算堆的结束地址（base + 32MB）
    void* end = ADDR_ADD(base, heap_size);
    // 第三步：调用brk(end)，将堆顶扩展到end地址（申请32MB堆空间）
    end = (void*)brk(end);
    if (!end)
        return 0;
#endif
    // 把堆空间的起始地址base，强制转为heap_header指针（第一个堆块的头部）
    header = (heap_header*)base; 
    header->size = heap_size;
    header->type = HEAP_BLOCK_FREE;
    header->next = NULL;
    header->prev = NULL;
    list_head = header;
    return 1;
}
