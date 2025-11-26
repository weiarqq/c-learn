
// 整数转任意进制字符串
// 将整数 n 转换为 radix 进制 的字符串，存储到 str 指向的字符数组中，返回指向结果字符串的指针（即 str 的起始地址）。
char* itoa(int n, char* str, int radix)
{
    char digit[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char* p = str;
    char* head = str;
    if (!p || radix < 2 || radix > 36)
        return p;
    if (radix != 10 && n < 0)
        return p;
    // 特殊情况：n=0，直接写入'0'和结束符
    if (n == 0)
    {
        *p++ = '0'; // 写入'0'，p指针后移, *p = '0' p++;
        *p = 0;     // 写入字符串结束符'\0'（0等价于'\0'） *p = '\0' 字符串以'\0'结尾
        return p;
    }
    if (radix == 10 && n < 0)
    {
        *p++ = '-';
        n = -n;
    }

    // 核心逻辑：循环取n的radix进制余数，写入映射表对应的字符
    while (n)
    {
        // n%radix得到当前位的数值（0~radix-1），从digit取对应字符
        *p++ = digit[n % radix];
        // n除以radix，去掉已处理的最低位
        n /= radix;
    }
    *p = 0; // 结尾
    // 反转字符串：将逆序的字符转正（head是起始地址，p是结束符前一位）
    // --p p当前位于'\0'
    for (--p; head < p; ++head, --p)
    {
        char temp = *head;
        *head = *p;
        *p = temp;
    }
    return str;

}

//比较两个字符串 src 和 dst 的字典序（ASCII 码顺序），返回结果：
int strcmp (const char* src, const char* dst)
{
    int ret = 0 ;
    //用 unsigned char 转换是为了正确处理 ASCII 码中的扩展字符（0x80~0xFF），避免符号位干扰。
    unsigned char* p1 = (unsigned char*)src;
    unsigned char* p2 = (unsigned char*)dst;
    // 循环条件：1. 当前字符相等（ret=*p1-*p2=0）；2. dst未到结束符（*p2 != '\0'）
    while ( ! (ret = *p1 - *p2) && *p2)
        ++p1, ++p2;
    if ( ret < 0 )
        ret = -1 ;
    else if ( ret > 0 )
        ret = 1 ;
    return( ret );
}

//字符串拷贝
char *strcpy(char *dest, const char *src)
{
    char* ret = dest;
    while(*src)
        *dest++ = *src++;
    *dest = '\0';
    return ret;

}

// 获取字符串长度
unsigned strlen(const char *str)
{
    int cnt = 0;
    if (!str)
        return 0;
    // \0结尾
    for (; *str != '\0'; ++str)
        ++cnt;
    return cnt;
}