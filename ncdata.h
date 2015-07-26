#ifndef NC_DATA_H
#define NC_DATA_H

#include <stddef.h>

typedef enum { GEO, CAT, TIME } NcDataType;

typedef struct NcValueChain {
    NcDataType type;
    size_t num;
    void *data; //array
} NcValueChain;

typedef struct GeoData {
    int x;
    int y;
    int z;
} GeoData;

typedef struct CatData {
    int category;
} CatData;

NcValueChain *createGeoChain(int x, int y, size_t depth);
NcValueChain *createCatChain(int category);
void printChain(NcValueChain *self);

#endif
