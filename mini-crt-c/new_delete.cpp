extern "C" void* malloc(unsigned int);
extern "C" void free(void*);

// operator 操作符重载
void* operator new(unsigned long size){
    return malloc(size);
}

void operator delete(void* p){
    free(p);
}

void* operator new[](unsigned long size){
    return malloc(size);
}

void operator delete[](void* p){
    free(p);
}
