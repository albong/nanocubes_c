#include "ncmem.h"

static unsigned long long BYTE_COUNT = 0;

void addMemToCount(size_t count){
    BYTE_COUNT += count;
}

unsigned long long getMemCount(){
    unsigned long long result = BYTE_COUNT;
    BYTE_COUNT = 0;
    return result;
}

