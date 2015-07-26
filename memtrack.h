#ifndef MEMTRACK_H
#define MEMTRACK_H

#include <stddef.h>

unsigned long long getMemUsage();
void *trackMalloc(size_t size);
void *trackRealloc(void *ptr, size_t size);
void trackFree(void *ptr);

#endif
