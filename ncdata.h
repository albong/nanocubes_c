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
    int x;
    int y;
    int z;
} GeoData;

typedef struct CatData {
    int category;
} CatData;

typedef struct TimeData {
    int time;
    unsigned long long count;
} TimeData;

NcValueChain *createChain(NcData *self, NcDataType type, int dimension);
NcValueChain *createGeoChain(int x, int y, size_t depth);
NcValueChain *createCatChain(int category);
void freeChain(NcValueChain *self);
void printChain(NcValueChain *self);

NcData *newGeoData(int x, int y, int z);
NcData *newCatData(int category);
NcData *newTimeData(int time, unsigned long long count);
NcData *getDataAtInd(NcData *self, int index);

#endif
