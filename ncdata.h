#ifndef NC_DATA_H
#define NC_DATA_H

#include <stddef.h>

typedef enum { GEO, CAT, TIME } NcDataType;

typedef struct NcValueChain {
    NcDataType type;
    size_t num;
    void *data; //array
} NcValueChain;

typedef struct NcData {
    void *data;
    struct NcData *next;
} NcData;

typedef struct GeoData {
    unsigned long long x;
    unsigned long long y;
    unsigned char z;
} GeoData;

typedef struct CatData {
    unsigned long long category;
} CatData;

typedef struct TimeData {
    size_t time;
    unsigned long long count;
} TimeData;

NcValueChain *createChain(NcData *self, NcDataType type, int dimension);
NcValueChain *createGeoChain(unsigned long long x, unsigned long long y, unsigned char depth);
NcValueChain *createCatChain(unsigned long long category);
void freeChain(NcValueChain *self);
void printChain(NcValueChain *self);

NcData *newGeoData(unsigned long long x, unsigned long long y, unsigned char z);
NcData *newCatData(unsigned long long category);
NcData *newTimeData(size_t time, unsigned long long count);
NcData *getDataAtInd(NcData *self, size_t index);
void freeData(NcData *self);

#endif
