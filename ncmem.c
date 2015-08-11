#include "ncmem.h"

static unsigned long long BYTE_COUNT = 0;

void addMemToCount(size_t count){
    BYTE_COUNT += count;
}

unsigned long long getMemCount(){
    return BYTE_COUNT;
}

void clearMemCount(){
    BYTE_COUNT = 0;
}

