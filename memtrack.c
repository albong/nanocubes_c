#include "memtrack.h"
#include <stdlib.h>

static unsigned long long count;
static const int SIZE = sizeof(size_t);

unsigned long long getMemUsage(){
    return count; 
}

void *trackMalloc(size_t size){
    count += size + SIZE;
    void *ptr = malloc(size + SIZE);
    *((size_t *)ptr) = size;
    return (char *)ptr + SIZE;
}

//not convinced works yet
void *trackRealloc(void *ptr, size_t size){
    void *realPtr = (char *)ptr - SIZE;
    count += (size - *((size_t *)realPtr));
    void *result = realloc(realPtr, size + SIZE);
    *((size_t *)result) = size;
    return (char *)result + SIZE;
}

void trackFree(void *ptr){
    void *realPtr = (char *)ptr - SIZE;
    count -= (SIZE + *((size_t *)realPtr));
    free(realPtr);
}
